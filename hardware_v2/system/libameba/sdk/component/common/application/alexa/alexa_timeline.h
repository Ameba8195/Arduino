#ifndef _ALEXA_TIMELINE_H_
#define _ALEXA_TIMELINE_H_

#define ALEXA_TIMELINE_RECORD_START           (1)
#define ALEXA_TIMELINE_RECORD_STOP            (2)
#define ALEXA_TIMELINE_MP3_START              (3)
#define ALEXA_TIMELINE_MP3_STOP               (4)
#define ALEXA_TIMELINE_EXPECT_SPEECH          (5)
#define ALEXA_TIMELINE_EXPECT_SPEECH_TIMEOUT  (6)

void alexa_timeline_add_timestamp(uint32_t event, uint32_t data);

uint32_t alexa_timeline_get_response_delay();

/**
 * Get expect speech status
 *
 * @return -1: unknown, check it later
 *          0: OK, no timeout happen
 *          1: timeout detected
 */
int32_t alexa_timeline_get_expect_speech_status();

uint32_t alexa_timeline_get_dialog_timeout();

void alexa_timeline_reset_inactivity_time();

uint32_t alexa_timeline_get_inactivity_time();

#endif
