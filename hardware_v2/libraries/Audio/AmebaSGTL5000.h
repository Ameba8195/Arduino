#ifndef _AMEBA_SGTL5000_H_
#define _AMEBA_SGTL5000_H_

#if defined(BOARD_RTL8710)
#error "RTL8710 do not support SDIO"
#endif

#include <inttypes.h>

#define AUDIO_INPUT_LINEIN  0
#define AUDIO_INPUT_MIC     1

class SGTL5000Class
{
public:
    SGTL5000Class();
    ~SGTL5000Class();

    uint8_t begin();
    uint8_t muteHeadphone();
    uint8_t unmuteHeadphone();
    uint8_t muteLineout();
    uint8_t unmuteLineout();
    uint8_t inputSelect(uint8_t type);
    uint8_t setVolume(float val);
    uint8_t micGain(uint16_t dB);

private:
};

extern SGTL5000Class SGTL5000;

#endif
