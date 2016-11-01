/** @file NfcTag.h */

/** 
 * @defgroup NfcTag NfcTag
 * NFC Tag API
 * @{
 */

#ifndef _NFC_TAG_H_
#define _NFC_TAG_H_

#include "Stream.h"

/** The length of NFC Tag UID */
#define NFC_UID_LEN 7

/** 
 * The first byte of UID is identified as Realtek
 */
#define RTK_NFC_UID 0x58

/*
 * @brief Total available memroy size
 *
 * In NFC Tag, 1 page equals 4 bytes. In Ameba there are 36 pages available which means there are total 36 x 4 = 144 bytes.
 * These are used as tag header, payload header, and payload itself.
 */
#define NFC_MAX_PAGE_NUM 36

/**
 * @brief The maximum number of NDEF message available
 *
 * In general case, 2 NDEF message is enough for most application.
 */
#define NFC_MAX_NDEF_NUM 2

/**
 * @defgroup tnf_message_type tnf_message_type
 *
 * TNF(Type Name Format) bit fields.
 *
 * @{
 */
/** The first NDEF message. */
#define TNF_MESSAGE_BEGIN                0x80
/** The last NDEF message. */
#define TNF_MESSAGE_END                  0x40
/** For cases that NDEF message is seperated into several parts. It shell not be used in Ameba */
#define TNF_MESSAGE_CHUNK_FLAG           0x20
/** For message less than 256 bytes. In Ameba it's always set to 1. */
#define TNF_MESSAGE_SHORT_RECORD         0x10
/** If this is set, ther there will be 1 byte ID information append after field payload length. In Ameba it's not enabled */
#define TNF_MESSAGE_ID_LENGTH_IS_PRESENT 0x08
/** @} */ // end of tnf_message_type

/**
 * @defgroup tnf_value tnf_value
 *
 * TNF(Type Name Format) value.
 *
 * @{
 */
/** empty */
#define TNF_EMPTY                        0x00
/** NFC Forum well-known type. */
#define TNF_WELL_KNOWN                   0x01
/** Media-type as defined in RFC 2046*/
#define TNF_MIME_MEDIA                   0x02
/** Absolute URI as defined in RFC 3986 */
#define TNF_ABSOLUTE_URI                 0x03
/** NFC Forum external type. */
#define TNF_EXTERNAL_TYPE                0x04
/** unknown */
#define TNF_UNKNOWN                      0x05
/** unchanged */
#define TNF_UNCHANGED                    0x06
/** reserved */
#define TNF_RESERVED                     0x07
/** @} */ // end of tnf_value

/**
 * @defgroup ndef_text_coding ndef_text_coding
 *
 * The coding of NDEF text type
 *
 * @{
 */
/** utf8 (default value in Ameba) */
#define NDEF_TEXT_ENCODE_UTF8  0x00
/** utf16 */
#define NDEF_TEXT_ENCODE_UTF16 0x80
/** @} */ // end of ndef_text_coding

/** language scheme for English */
#define NDEF_IANA_ENGLISH "en"

/**
 * @defgroup rtd_uri_type rtd_uri_type
 *
 * URI type in RTD(Record Type Definition)
 *
 * @{
 */
/** "http://www." */
#define RTD_URI_HTTP_WWW      0x01
/** "https://www." */
#define RTD_URI_HTTPS_WWW     0x02
/** "http://" */
#define RTD_URI_HTTP          0x03
/** "https://" */
#define RTD_URI_HTTPS         0x04
/** "tel:" */
#define RTD_URI_TEL           0x05
/** "mailto:" */
#define RTD_URI_MAILTO        0x06
/** @} */ // end of rtd_uri_type

#define NFC_THREAD_STACK_SIZE 1024

/**
 * @struct NDEF
 */
struct NDEF {
    /** TNF Flag */
    unsigned char TNF_flag;
    /** The length of payload_type string */
    unsigned char type_len;
    /** The length of payload */
    unsigned char payload_len;
    /** The payload_type in string format */
    unsigned char *payload_type;
    /** Payload */
    unsigned char *payload;
};

/**
 * @class NfcTagClass NfcTag.h
 * @brief Nfc Tag class
 *
 * This class contain the tag content include header and payload. It also handles the R/W interrupts.
 */
class NfcTagClass {

// public function members
public:

    /*
     * @brief The constructor
     *
     * @param[in] nfcid The 7 bytes of UID
     */
	NfcTagClass( unsigned char nfcid[NFC_UID_LEN] );

    /**
     * @brief start nfc firmware
     */
	void begin();

    /*
     * @brief stop nfc firmware
     */
    void end();

    /**
     * @brief Create and append a text NDEF record
     *
     * @param[in] text The text message
     * @param[in] encodeType The encode type in utf8 or utf16
     * @param[in] IANALanguageCode The coding of language
     */
    void appendRtdText(const char *text, unsigned char encodeType, const char *IANALanguageCode);

    /**
     * @brief Create and append a text NDEF record
     *
     * Create and append a text NDEF record. By default it use utf8 with language English.
     *
     * @param[in] text The text message
     */
    void appendRtdText(const char *text);

    /**
     * @brief Create and append RTD URI record
     *
     * @param[in] text The URI string
     * @param[in] uriIdentifierCode The URI code correspond to desired URI header
     */
    void appendRtdUri(const char *text, unsigned char uriIdentifierCode);

    /**
     * @brief Create and append RTD URI record
     *
     * Create and append a RTD URI record. By default it use "http://www." as URI header.
     *
     * @param[in] text The URI string
     */
    void appendRtdUri(const char *text);

    /**
     * @brief Create and append V-Card record
     *
     * Create and append a V-Card record.
     * Since the V-Card use richful string format, it is suggest V-Card version 2.1 which has less must required fields.
     *
     * @param[in] vcard The V-Card string content
     * @param[in] vcard_len The length of the V-Card content
     */
    void appendVcard(const char *vcard, int vcard_len);

    /**
     * @brief Create and append AAR record
     *
     * AAR (Android Application Record) is a NDEF record only use of Android framework.
     * When a Android device with NFC reader function detect this tag with AAR, it will send an intent and lunch this application.
     *
     * @param appName The application name in Android google play
     */
    void appendAndroidPlayApp(const char *appName);

    /**
     * @brief Clear catched NDEF records in NfcTag object
     *
     * @note Clear NDEF only clear cached NDEF records in this object. It won't affect the NFC firmware
     */
    void clearNdefMessage();

    /**
     * @brief Check if UID field is valid
     *
     * It will check the content of nfc_tag_content and verify UID field.
     *
     * @return Return true if valid. And false vise sersa.
     */
    bool isUidValid();

    /**
     * @brief Convert the cached NDEF messages into NFC Tag Type2 format
     *
     * This will convert the cached NDEF message into raw data and stored in nfc_tag_content.
     * At this point the nfc_tag_conten is not updated to NFC firmware
     */
    void convertNdefToRaw();

    /**
     * @brief Conver the content of nfc_tag_content into NDEF messages
     *
     * It tries to extract and convert the content of nfc_tag_content into NDEF message.
     * We expect the content of nfc_tag_content is in the format of NFC Tag Type2
     */
    void convertRawToNdef();

    /**
     * @brief Update the content of nfc_tag_content into NFC firmware
     */
    void updateRawToCache();

    /*
     * @brief Return last system timestamp that NFC cache being updated
     *
     * @return The timestamp in unit of millisecond
     */
    uint32_t getLastUpdateTimestamp();

    /**
     * @brief Set protection that avoid a NFC reader to modify NFC Tag content on Ameba
     *
     * By default the content of Ameba's Tag is readable and writable.
     * To avoid being changed from a NFC reader, we can set write protection (in software solution).
     *
     * @param[in] enable Set to true if enable software write protection.
     */
    void setWriteProtect(bool enable);

    /**
     * @brief Return current stored NDEF messages size (It should be the same with Tag content)
     *
     * @return The NDEF message size
     */
    unsigned char getNdefSize();

    /**
     * @brief Return current stored NDEF messages (It should be the same with Tag content)
     *
     * @return The NDEF struct array pointer
     */
    const struct NDEF *getNdefData();

public:

    /**
     * @brief The raw data of NFC tag content stored in Ameba.
     */
	uint32_t nfc_tag_content[NFC_MAX_PAGE_NUM];


/** @cond HIDDEN_SYMBOLS */

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

/** @endcond */

};

extern NfcTagClass NfcTag;

#endif

/** @} */ // end of group NfcTag