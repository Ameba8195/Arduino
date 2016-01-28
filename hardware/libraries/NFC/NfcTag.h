#ifndef _NFC_TAG_H_
#define _NFC_TAG_H_

#include "Stream.h"

#define NFC_UID_LEN 7
#define RTK_NFC_UID 0x58

#define NFC_MAX_PAGE_NUM 36

#define NFC_MAX_NDEF_NUM 2

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

    /* Store nfc raw data by pages */
	static uint32_t nfc_tag_content[NFC_MAX_PAGE_NUM];

    bool isUidValid();

    /* Convert NDEF messages to NFC Tag Type2 format */
    void convertNdefToRaw();

    /* Return last system timestamp that NFC cache being updated */
    uint32_t getLastUpdateTimestamp();

    /* Set protection that avoid NFC reader to modify NFC Tag content */
    void setWriteProtect(bool enable);

private:
    /* Helper function to fill TNF+flag field of NDEF message */
    void addTnfRecord(unsigned char tnfType);

    /* A callback function when NFC reader perform write actions.
     * It's called for every written page, so keep this function process fast enough.
     * Otherwise it will broke the write process.
     */
    static void nfcWriteListener(void *arg, unsigned int page, uint32_t pgdat);

    /* A callback function when NFC reader perform any actions to Tag */
    static void nfcEventListener(void *arg, unsigned int event);

    /* A internal message handler */
    static void nfcThread(void const *argument);

    /* Store nfc object address */
	static void* pNfcTag;

    /* Store information that which pages has been modified */
	static unsigned char nfc_tag_dirty[NFC_MAX_PAGE_NUM];

    /* ndef struct size */
    unsigned char ndef_size;

    /* ndef struct data */
    struct NDEF ndef_msg[NFC_MAX_NDEF_NUM];

    /* thread id of nfc internal message handler */
    static void *nfctid;

    static uint32_t lastUpdateTimestamp;

    static bool writeProtect;
};

extern NfcTagClass NfcTag;

#endif

