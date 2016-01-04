#ifndef NFC_H
#define NFC_H

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

struct NDEF {
    unsigned char TNF_flag;
    unsigned char type_len;
    unsigned char payload_len;
    unsigned char *payload_type;
    unsigned char *payload;
};

class NfcTagClass {
public:
	NfcTagClass( unsigned char nfcid[NFC_UID_LEN] );
	void begin();
    void end();

    void appendRtdText(const char *text, unsigned char encodeType, const char *IANALanguageCode);
    void appendRtdText(const char *text);

    void appendRtdUri(const char *text, unsigned char uriIdentifierCode);
    void appendRtdUri(const char *text);

    void appendVcard(const char *vcard, int vcard_len);

    void appendAndroidPlayApp(const char *appName);

private:
    void addTnfRecord(unsigned char tnfType);
    void convertNdefToRaw();

	void* pNfcTag;

	static uint32_t nfc_tag_content[NFC_MAX_PAGE_NUM];
	static unsigned char nfc_tag_dirty[NFC_MAX_PAGE_NUM];

    unsigned int page_size;
    unsigned int raw_size;  // raw data of tag content which from page 4

    unsigned char ndef_size;
    struct NDEF ndef_msg[NFC_MAX_NDEF_NUM];
};

extern NfcTagClass NfcTag;

#endif

