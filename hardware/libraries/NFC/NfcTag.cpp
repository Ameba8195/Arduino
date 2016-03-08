#include "NfcTag.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nfc_api.h"
#include "cmsis_os.h"

extern int nfc_free(nfctag_t *obj);

nfctag_t nfctag;

#ifdef __cplusplus
}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) do { if ((x) != NULL) {free(x); x=NULL;} } while(0)
#endif

unsigned char nfc_default_uid[7] = {
    RTK_NFC_UID, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
};

void nfcThread(void const *argument);
void nfcWriteListener(void *arg, unsigned int page, uint32_t pgdat);
void nfcEventListener(void *arg, unsigned int event);

NfcTagClass::NfcTagClass(unsigned char uid[NFC_UID_LEN]) {

    unsigned int page_size = 0;

    // initialize class members
    ndef_size = 0;
    nfctid = NULL;
    lastUpdateTimestamp = 0;
    pNfcTag = NULL;
    writeProtect = false;

    memset(nfc_tag_content, 0, NFC_MAX_PAGE_NUM * sizeof(unsigned int));
    memset(nfc_tag_dirty, 0, NFC_MAX_PAGE_NUM * sizeof(unsigned char));

    // calculate Block Check Character
    unsigned char bcc[2];
    bcc[0] = 0x88 ^ uid[0] ^ uid[1] ^ uid[2];
    bcc[1] = uid[3] ^ uid[4] ^ uid[5] ^ uid[6];

    // generate header
    nfc_tag_content[page_size++] = ((unsigned int)uid[0] <<  0) |
                                   ((unsigned int)uid[1] <<  8) |
                                   ((unsigned int)uid[2] << 16) |
                                   ((unsigned int)bcc[0] << 24);
    nfc_tag_content[page_size++] = ((unsigned int)uid[3]) <<  0 |
                                   ((unsigned int)uid[4]) <<  8 |
                                   ((unsigned int)uid[5]) << 16 |
                                   ((unsigned int)uid[6]) << 24;
    nfc_tag_content[page_size++] = ((unsigned int)bcc[1]) <<  0;
    nfc_tag_content[page_size++] = 0x001211E1;

    pNfcTag = (void *)&nfctag;
}

void NfcTagClass::begin() {
    osThreadDef_t nfc_thread_def = {
        nfcThread,
        osPriorityRealtime,
        1,
        NFC_THREAD_STACK_SIZE,
        "nfcthread"
    };
    nfctid = osThreadCreate (&nfc_thread_def, this);

    convertNdefToRaw();
    nfc_init ((nfctag_t *)pNfcTag, nfc_tag_content);
    nfc_event((nfctag_t *)pNfcTag, nfcEventListener, this, 0xFF);
    nfc_write((nfctag_t *)pNfcTag, nfcWriteListener, this);
    lastUpdateTimestamp = osKernelSysTick();
}

void NfcTagClass::end() {
    clearNdefMessage();
    nfc_free ((nfctag_t *)pNfcTag);
}

void NfcTagClass::appendRtdText(const char *text, unsigned char encodeType, const char *IANALanguageCode) {
    int text_len, language_code_len;

    addTnfRecord(TNF_WELL_KNOWN);

    ndef_msg[ndef_size].type_len = 1;

    ndef_msg[ndef_size].payload_type = (unsigned char *)malloc( 1 );
    ndef_msg[ndef_size].payload_type[0] = 0x54; // RTD Text

    text_len = strlen(text);
    language_code_len = strlen(IANALanguageCode);
    ndef_msg[ndef_size].payload_len = 1 + language_code_len + text_len;

    ndef_msg[ndef_size].payload = (unsigned char *)malloc( ndef_msg[ndef_size].payload_len );
    ndef_msg[ndef_size].payload[0] = encodeType + language_code_len;
    memcpy( &(ndef_msg[ndef_size].payload[1]), IANALanguageCode, language_code_len );
    memcpy( &(ndef_msg[ndef_size].payload[1 + language_code_len]), text, text_len);

    ndef_size++;
}

void NfcTagClass::appendRtdText(const char *text) {
    appendRtdText(text, NDEF_TEXT_ENCODE_UTF8, NDEF_IANA_ENGLISH);
}

void NfcTagClass::appendRtdUri(const char *uri, unsigned char uriIdentifierCode) {
    int uri_len;

    addTnfRecord(TNF_WELL_KNOWN);

    ndef_msg[ndef_size].type_len = 1;

    ndef_msg[ndef_size].payload_type = (unsigned char *)malloc( 1 );
    ndef_msg[ndef_size].payload_type[0] = 0x55; // RTD URI

    uri_len = strlen(uri);
    ndef_msg[ndef_size].payload_len = 1 + uri_len;

    ndef_msg[ndef_size].payload = (unsigned char *)malloc( ndef_msg[ndef_size].payload_len );
    ndef_msg[ndef_size].payload[0] = uriIdentifierCode;
    memcpy( &(ndef_msg[ndef_size].payload[1]), uri, uri_len );

    ndef_size++;
}

void NfcTagClass::appendRtdUri(const char *uri) {
    appendRtdUri(uri, RTD_URI_HTTP_WWW);
}

void NfcTagClass::appendAndroidPlayApp(const char *appName) {

    char *aar_record = "android.com:pkg";

    addTnfRecord(TNF_EXTERNAL_TYPE);

    ndef_msg[ndef_size].type_len = strlen(aar_record);
    ndef_msg[ndef_size].payload_type = (unsigned char *)malloc(ndef_msg[ndef_size].type_len);
    memcpy( ndef_msg[ndef_size].payload_type, aar_record, ndef_msg[ndef_size].type_len );

    ndef_msg[ndef_size].payload_len = strlen(appName);
    ndef_msg[ndef_size].payload = (unsigned char *)malloc( ndef_msg[ndef_size].payload_len );
    memcpy( ndef_msg[ndef_size].payload, appName, ndef_msg[ndef_size].payload_len);

    ndef_size++;
}

void NfcTagClass::appendVcard(const char *vcard, int vcard_len) {
    char *type_vcard = "text/x-vCard";

    addTnfRecord(TNF_MIME_MEDIA);

    ndef_msg[ndef_size].type_len = strlen(type_vcard);
    ndef_msg[ndef_size].payload_type = (unsigned char *)malloc(ndef_msg[ndef_size].type_len);
    memcpy( ndef_msg[ndef_size].payload_type, type_vcard, ndef_msg[ndef_size].type_len );

    ndef_msg[ndef_size].payload_len = vcard_len;
    ndef_msg[ndef_size].payload = (unsigned char *)malloc( ndef_msg[ndef_size].payload_len );
    memcpy( ndef_msg[ndef_size].payload, vcard, vcard_len);

    ndef_size++;
}

void NfcTagClass::addTnfRecord(unsigned char tnfType) {
    if (ndef_size == 0) {
        ndef_msg[ndef_size].TNF_flag = TNF_MESSAGE_BEGIN | TNF_MESSAGE_END | TNF_MESSAGE_SHORT_RECORD | tnfType;
    } else {
        ndef_msg[ndef_size].TNF_flag = TNF_MESSAGE_END | TNF_MESSAGE_SHORT_RECORD | tnfType;
        ndef_msg[ndef_size-1].TNF_flag &= ~TNF_MESSAGE_END;
    }
}

void NfcTagClass::clearNdefMessage() {
    int i;
    for (i=0; i<ndef_size; i++) {
        SAFE_FREE( ndef_msg[i].payload_type );
        SAFE_FREE( ndef_msg[i].payload );
    }
    ndef_size = 0;
}

void NfcTagClass::convertNdefToRaw() {
    int i, idx, ndef_idx;
    unsigned char buf[NFC_MAX_PAGE_NUM * 4];

    if (ndef_size == 0) {
        return;
    }

    memset( buf, 0, NFC_MAX_PAGE_NUM * 4 );

    buf[0] = 0x03; // 0x03 = TLV Tag type: NDEF
    idx = 2;
    for ( ndef_idx = 0; ndef_idx < ndef_size; ndef_idx++ ) {
        buf[idx++] = ndef_msg[ndef_idx].TNF_flag;
        buf[idx++] = ndef_msg[ndef_idx].type_len;
        buf[idx++] = ndef_msg[ndef_idx].payload_len;
        for ( i = 0; i < ndef_msg[ndef_idx].type_len; i++ ) {
            buf[idx++] = ndef_msg[ndef_idx].payload_type[i];
        }
        for ( i = 0; i < ndef_msg[ndef_idx].payload_len; i++ ) {
            buf[idx++] = ndef_msg[ndef_idx].payload[i];
        }
    }
    buf[1] = idx - 2; // TLV length
    buf[idx++] = 0xfe; // fe = terminal byte

    memcpy(&(nfc_tag_content[4]), buf, idx);
}

void NfcTagClass::convertRawToNdef() {
    int idx, tlv_len, ndefs_end;
    unsigned char has_find_ndefs = 0;

    unsigned char *buf = (unsigned char *)nfc_tag_content;

    clearNdefMessage();

    idx = 16;  // skip NFC Tag Type2 header
    while(!has_find_ndefs) {
        if (buf[idx] != 0x03) {
            idx += 1 + buf[idx+1];
            if (idx >= NFC_MAX_PAGE_NUM*4) {
                // fail to find TLV for NDEFs
                break;
            }
        } else {
            // now we find TLV for NDEFs

            tlv_len = buf[idx+1];
            idx += 2; // pointing to start address of NDEFs
            ndefs_end = idx + tlv_len;

            if (ndefs_end >= NFC_MAX_PAGE_NUM * 4) {
                // wrong format of TLV
                break;
            }

            has_find_ndefs = 1;
        }
    }

    if (!has_find_ndefs) {
        return;
    }

    while(idx < ndefs_end) {
        ndef_msg[ndef_size].TNF_flag = buf[idx++];
        ndef_msg[ndef_size].type_len = buf[idx++];
        ndef_msg[ndef_size].payload_len = buf[idx++];

        ndef_msg[ndef_size].payload_type = (unsigned char *)malloc( ndef_msg[ndef_size].type_len + 1 );
        memcpy( ndef_msg[ndef_size].payload_type, &(buf[idx]), ndef_msg[ndef_size].type_len );
        ndef_msg[ndef_size].payload_type[ ndef_msg[ndef_size].type_len ] = '\0';
        idx += ndef_msg[ndef_size].type_len;

        ndef_msg[ndef_size].payload = (unsigned char *)malloc( ndef_msg[ndef_size].payload_len + 1 );
        memcpy( ndef_msg[ndef_size].payload, &(buf[idx]), ndef_msg[ndef_size].payload_len );
        ndef_msg[ndef_size].payload[ ndef_msg[ndef_size].payload_len ] = '\0';
        idx += ndef_msg[ndef_size].payload_len;

        ndef_size++;
    }
}

void NfcTagClass::updateRawToCache() {
    if (!writeProtect) {
        // update to nfc cache from page 4
        nfc_cache_write((nfctag_t *)pNfcTag, &(nfc_tag_content[4]), 4, NFC_MAX_PAGE_NUM-4);
        lastUpdateTimestamp = osKernelSysTick();
    }
}

bool NfcTagClass::isUidValid() {
    bool uidvalid = true;

    unsigned char uid[7];
    unsigned char bcc[2];

    uid[0] = (unsigned char)((nfc_tag_content[0] & 0x000000FF) >>  0);
    uid[1] = (unsigned char)((nfc_tag_content[0] & 0x0000FF00) >>  8);
    uid[2] = (unsigned char)((nfc_tag_content[0] & 0x00FF0000) >> 16);
    bcc[0] = (unsigned char)((nfc_tag_content[0] & 0xFF000000) >> 24);
    uid[3] = (unsigned char)((nfc_tag_content[1] & 0x000000FF) >>  0);
    uid[4] = (unsigned char)((nfc_tag_content[1] & 0x0000FF00) >>  8);
    uid[5] = (unsigned char)((nfc_tag_content[1] & 0x00FF0000) >> 16);
    uid[6] = (unsigned char)((nfc_tag_content[1] & 0xFF000000) >> 24);
    bcc[1] = (unsigned char)((nfc_tag_content[2] & 0x000000FF) >>  0);

    // verify Block Check Character
    if (bcc[0] != (0x88 ^ uid[0] ^ uid[1] ^ uid[2])) {
        uidvalid = false;
    }
    if (bcc[1] != (uid[3] ^ uid[4] ^ uid[5] ^ uid[6])) {
        uidvalid = false;
    }

    return uidvalid;
}

uint32_t NfcTagClass::getLastUpdateTimestamp() {
    return lastUpdateTimestamp;
}

void NfcTagClass::setWriteProtect(bool enable) {
    writeProtect = enable;
}

unsigned char NfcTagClass::getNdefSize() {
    return ndef_size;
}

const struct NDEF *NfcTagClass::getNdefData() {
    return ndef_msg;
}

void nfcThread(void const *argument) {

    NfcTagClass *pTag;
    int i, modified_page_count;
    osEvent evt;

    pTag = (NfcTagClass *)argument;

    osSignalClear(pTag->nfctid, 0xFFFFFFFF);

    while(1) {
        evt = osSignalWait (0, osWaitForever);
        if (evt.status == osEventSignal && (evt.value.signals & NFC_EV_WRITE)) {
            osDelay(300);

            for (i = 4, modified_page_count = 0; i < NFC_MAX_PAGE_NUM && pTag->nfc_tag_dirty[i]; i++) {
                modified_page_count++;
            }

            if (!(pTag->writeProtect)) {
                // update to nfc cache from page 4
                nfc_cache_write((nfctag_t *)(pTag->pNfcTag), &((pTag->nfc_tag_content)[4]), 4, modified_page_count);
                pTag->lastUpdateTimestamp = osKernelSysTick();

                pTag->convertRawToNdef();
            }

            memset(pTag->nfc_tag_dirty, 0, NFC_MAX_PAGE_NUM);
            osSignalClear(pTag->nfctid, NFC_EV_WRITE);
        }
    }
}

void nfcWriteListener(void *arg, unsigned int page, uint32_t pgdat) {

    NfcTagClass *pTag;

    pTag = (NfcTagClass *)arg;

    (pTag->nfc_tag_content)[page] = pgdat;
    (pTag->nfc_tag_dirty)[page] = 1;
    if (pTag->nfctid != NULL) {
        osSignalSet(pTag->nfctid, NFC_EV_WRITE);
    }
}

void nfcEventListener(void *arg, unsigned int event) {
    switch(event) {
        case NFC_EV_READER_PRESENT:
            break;
        case NFC_EV_READ:
            break;
        case NFC_EV_WRITE:
            break;
        case NFC_EV_ERR:
            break;
        case NFC_EV_CACHE_READ:
            break;
    }
}

NfcTagClass NfcTag = NfcTagClass(nfc_default_uid);

