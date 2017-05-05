#ifdef __cplusplus
extern "C" {
#endif

#include "sgtl5000.h"

#ifdef __cplusplus
}
#endif

#include "AmebaSGTL5000.h"

SGTL5000Class::SGTL5000Class()
{
}

SGTL5000Class::~SGTL5000Class()
{
}

uint8_t SGTL5000Class::begin()
{
    sgtl5000_enable();
}

uint8_t SGTL5000Class::muteHeadphone()
{
    return sgtl5000_muteHeadphone();
}

uint8_t SGTL5000Class::unmuteHeadphone()
{
    return sgtl5000_unmuteHeadphone();
}

uint8_t SGTL5000Class::muteLineout()
{
    return sgtl5000_muteLineout();
}

uint8_t SGTL5000Class::unmuteLineout()
{
    return sgtl5000_unmuteLineout();
}

uint8_t SGTL5000Class::inputSelect(uint8_t type)
{
    return sgtl5000_inputSelect(type);
}

uint8_t SGTL5000Class::setVolume(float val)
{
    return sgtl5000_setVolume(val);
}

uint8_t SGTL5000Class::micGain(uint16_t dB)
{
    return sgtl5000_micGain(dB);
}

SGTL5000Class SGTL5000;

