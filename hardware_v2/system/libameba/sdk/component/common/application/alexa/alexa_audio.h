#ifndef _ALEXA_AUDIO_SERVICE_H_
#define _ALEXA_AUDIO_SERVICE_H_

#include <inttypes.h>
#include "alexa/alexa.h"

#define ALEXA_AUDIO_RX_BUFSIZE (1152 * 28 * 8)

/**
 * Make audio start recording and upload to Alexa AVS
 */
void alexa_audio_record_start(uint32_t timeout_ms);

/**
 * Make audio stop recording.
 */
void alexa_audio_record_stop();

/**
 * @return 0: Audio is not recording
 *         1: Audio is under recording
 */
int alexa_audio_is_recording();

/**
 * Push audio data into buffer. It will play audio when data is larger than some threshold.
 */
uint32_t alexa_audio_push_play(alexa_audio_t *mp3_data, uint8_t channel, uint8_t push_head);

void alexa_audio_set_volume(uint32_t volume);

void alexa_audio_play_resource(uint32_t res_id);

void alexa_audio_change_channel(uint8_t channel, uint8_t enable);

void mp3_data_default_destructor(alexa_audio_t *p_mp3_data);

#endif
