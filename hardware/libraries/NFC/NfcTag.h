#ifndef _NFC_TAG_H_
#define _NFC_TAG_H_

#include "Stream.h"

#define NFC_UID_LEN 7
#define RTK_NFC_UID 0x58

#define NFC_MAX_PAGE_NUM 36

// In most case, 2 is enough for most application. Modify it if you need more.
#define NFC_MAX_NDEF_NUM 2

#define TNF_MESSAGE_BEGIN                0x80
#define TNF_MESSAGE_END                  0x40
#define TNF_MESSAGE_CHUNK_FLAG           0x20
#define TNF_MESSAGE_SHORT_RECORD         0x10
#define TNF_MESSAGE_ID_LENGTH_IS_PRESENT 0x08

#define TNF_EMPTY                        0x00
#define TNF_WELL_KNOWN                   0x01
#define TNF_MIME_MEDIA                   0x02 // RFC 2046
#define TNF_ABSOLUTE_URI                 0x03 // RFC 3986
#define TNF_EXTERNAL_TYPE                0x04
#define TNF_UNKNOWN                      0x05
#define TNF_UNCHANGED                    0x06
#define TNF_RESERVED                     0x07

#define NDEF_TEXT_ENCODE_UTF8  0x00
#define NDEF_TEXT_ENCODE_UTF16 0x80

#define NDEF_IANA_ENGLISH "en"

#define RTD_URI_HTTP_WWW      0x01 // http://www.
#define RTD_URI_HTTPS_WWW     0x02 // https://www.
#define RTD_URI_HTTP          0x03 // http://
#define RTD_URI_HTTPS         0x04 // https://
#define RTD_URI_TEL           0x05 // tel:
#define RTD_URI_MAILTO        0x06 // mailto:

#define NFC_THREAD_STACK_SIZE 1024

struct NDEF {
    unsigned char TNF_flag;
    unsigned char type_len;
    unsigned char payload_len;
    unsigned char *payload_type;
    unsigned char *payload;
};

class NfcTagClass {

// public function members
public:

    /* constructor */
	NfcTagClass( unsigned char nfcid[NFC_UID_LEN] );

    /* start nfc firmware */
	void begin();

    /* stop nfc firmware */
    void end();

    /* append NDEF text message */
    void appendRtdText(const char *text, unsigned char encodeType, const char *IANALanguageCode);
    void appendRtdText(const char *text);

    /* append NDEF URI message */
    void appendRtdUri(const char *text, unsigned char uriIdentifierCode);
    void appendRtdUri(const char *text);

    /* append NDEF V-Card message */
    void appendVcard(const char *vcard, int vcard_len);

    /* append NDEF AAR message */
    void appendAndroidPlayApp(const char *appName);

    /* clear previous stored NDEF messages */
    void clearNdefMessage();

    /* check if UID checksum matches BCC */
    bool isUidValid();

    /* Convert NDEF messages to NFC Tag Type2 format */
    void convertNdefToRaw();

    /* Convert NFC Tag Type2 format to NDEF messages */
    void convertRawToNdef();

    /* Update NFC TAG content from nfc_tag_content[] */
    void updateRawToCache();

    /* Return last system timestamp that NFC cache being updated */
    uint32_t getLastUpdateTimestamp();

    /* Set protection that avoid NFC reader to modify NFC Tag content */
    void setWriteProtect(bool enable);

    /* Return current stored NDEF messages size (It should be the same with Tag content)*/
    unsigned char getNdefSize();

    /* Return current stored NDEF messages (It should be the same with Tag content) */
    const struct NDEF *getNdefData();

// public data members
public:

    /* Store nfc raw data by pages */
	uint32_t nfc_tag_content[NFC_MAX_PAGE_NUM];

// friend functions
public:
    /* A callback function when NFC reader perform write actions.
     * It's called for every written page, so keep this function process fast enough.
     * Otherwise it will broke the write process.
     */
    friend void nfcWriteListener(void *arg, unsigned int page, uint32_t pgdat);

    /* A callback function when NFC reader perform any actions to Tag */
    friend void nfcEventListener(void *arg, unsigned int event);

    /* A internal message handler */
    friend void nfcThread(void const *argument);


// private function members
private:

    /* Helper function to fill TNF+flag field of NDEF message */
    void addTnfRecord(unsigned char tnfType);

// private data members
private:

    /* Store nfc object address */
	void* pNfcTag;

    /* Store information that which pages has been modified */
	unsigned char nfc_tag_dirty[NFC_MAX_PAGE_NUM];

    /* ndef struct data */
    struct NDEF ndef_msg[NFC_MAX_NDEF_NUM];

    /* ndef struct size */
    unsigned char ndef_size;

    /* thread id of nfc internal message handler */
    void *nfctid;

    uint32_t lastUpdateTimestamp;

    bool writeProtect;
};

extern NfcTagClass NfcTag;

#endif

