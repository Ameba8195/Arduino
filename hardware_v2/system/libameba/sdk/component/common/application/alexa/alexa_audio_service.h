#ifndef _ALEXA_AUDIO_SERVICE_H_
#define _ALEXA_AUDIO_SERVICE_H_

#include <inttypes.h>

void init_alexa_audio_service(void (*init_codec_callback)(), uint8_t *rx_buf, uint32_t rx_bufsize, uint8_t *tx_buf, uint32_t tx_bufsize);

void alexa_audio_record_start();
void alexa_audio_record_stop();
int alexa_audio_is_recording();
void alexa_audio_force_play();
uint32_t alexa_audio_push_play(uint8_t *buf, uint32_t bufsize);
void alexa_audio_set_tx_buffering_threshold(uint32_t threshold);

#endif
