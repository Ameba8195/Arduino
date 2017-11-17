#ifndef _ALEXA_SERVICE_H_
#define _ALEXA_SERVICE_H_

#include <inttypes.h>

/**
 * <!-- ++++++++++++++++ group alexa_channel ++++++++++++++++ -->
 * @defgroup alexa_res alexa resource control
 *     Channel priority: DIALOG > ALERTS > CONTENT
 * @{
 */
#define ALEXA_AUDIO_CHANNEL_NONE    (0x00)
#define ALEXA_AUDIO_CHANNEL_DIALOG  (0x01)
#define ALEXA_AUDIO_CHANNEL_ALERTS  (0x02)
#define ALEXA_AUDIO_CHANNEL_CONTENT (0x04)
/** 
 * @}
 * <!-- ---------------- group alexa_channel ---------------- -->
 */

#define ALEXA_AUDIO_FLAG_START            (0x0001)
#define ALEXA_AUDIO_FLAG_PLAY             (0x0002)
#define ALEXA_AUDIO_FLAG_NEARLY_FINISHED  (0x0004)
#define ALEXA_AUDIO_FLAG_EOF              (0x0008)

#define ALEXA_AUDIO_FLAG_DIALOG           (0x0010)
#define ALEXA_AUDIO_FLAG_CONTENT          (0x0020)

#define ALEXA_AUDIO_FLAG_TYPE_MP3         (0x0100)
#define ALEXA_AUDIO_FLAG_TYPE_TS          (0x0200)

/** Alexa audio resource ID: NONE */
#define ALEXA_AUDIO_RES_NONE  (0)

/** Alexa audio resource ID: ALARM */
#define ALEXA_AUDIO_RES_ALARM (1)

typedef struct {
    uint8_t *refresh_token;
    uint32_t refresh_token_len;

    char *avs_client_id;
    uint32_t avs_client_id_len;

    char *avs_client_secret;
    uint32_t avs_client_secret_len;

    char *avs_hp2_host;
    uint32_t avs_hp2_host_len;
} alexa_context_t;

typedef struct _alexa_audio_t {
    int32_t id;
    uint8_t *data;
    uint32_t data_len;
    uint32_t flag;
    void (*destructor)(struct _alexa_audio_t *self);
} alexa_audio_t;

/* ++++++++++++++++ Alexa AVS Service ++++++++++++++++ */

/**
 * @return 0 if Alexa AVS state is not ready
 *         1 if Alexa AVS state is ready
 */
uint32_t alexa_avs_state();

/**
 *
 * @return 0 if Alexa AVS is not in upload state and ready to upload
 *         not 0 if Alexa AVS is under uploading
 */
uint8_t alexa_avs_upload_state();

/**
 * Set AVS volume
 *
 * @param volume 0~100 which mapping to AVS 0~10 volume value
 */
void alexa_avs_set_volume(uint8_t volume);

/**
 * get AVS volume
 *
 * @return 0~100 which mapping to AVS 0~10 volume value
 */
uint8_t alexa_avs_get_expect_volume();

/**
 * set AVS mute
 *
 * @param 1 for mute, 0 for unmute
 */
void alexa_avs_set_mute(uint8_t mute);

/**
 * get AVS mute
 *
 * @return mute state
 */
uint8_t alexa_avs_get_expect_mute();

/**
 * Set locale in SettingsUpdated event
 *
 * Valid values are "en-US", "en-GB", and "de-DE"
 */
void alexa_avs_set_locale(char *locale);

/**
 * Initialize AVS service
 */
void init_alexa_avs_service(alexa_context_t *context);

/* ---------------- Alexa AVS Service ---------------- */

/* ++++++++++++++++ Alexa Audio Service ++++++++++++++++ */

/**
 * Push PCM source data to Alexa and process it
 *
 * @return  0   Success
 *         !0   fail
 */
uint8_t alexa_audio_handle_rx(uint8_t *data, uint32_t len);

/**
 * Initialize alexa audio service
 *
 * @param audio_tx_callback   callback function which is there is audio tx data to be played
 *
 * @param set_volume_callback callback function when volume is changed from cloud size
 */
void init_alexa_audio_service(
    uint32_t (*audio_tx_callback)(alexa_audio_t *data, uint8_t channel, uint8_t push_head),
    void (*audio_change_channel_callback)(uint8_t channel, uint8_t enable),
    void (*set_volume_callback)(uint32_t volume),
    void (*audio_play_resource_callback)(uint32_t res_id)
);

/* ---------------- Alexa Audio Service ---------------- */

#endif
