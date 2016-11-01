#ifdef __cplusplus
extern "C" {

#include "timer_api.h"
extern u32 ConfigDebugErr;

}
#endif

#include "GTimer.h"

uint32_t timer_mapping[GTIMER_MAX] = {
    TIMER0, // GTimer 2, share with PWM_3
    TIMER1, // GTimer 3, share with PWM_0
    TIMER2, // GTimer 4, share with PWM_1
    TIMER3, // GTimer 5, share with PWM_2
    TIMER4  // GTimer 0, share with software-RTC functions
};

gtimer_t objGTimers[GTIMER_MAX];

void GTimerClass::begin(uint32_t timerid, uint32_t duration_us, void (*handler)(uint32_t), bool periodical, uint32_t userdata)
{
    uint32_t tid;
    uint32_t backup_ConfigDebugErr;

    if (timerid > GTIMER_MAX ) {
        return;
    }

    backup_ConfigDebugErr = ConfigDebugErr;
    ConfigDebugErr = 0x00000000;

    tid = timer_mapping[timerid];
    gtimer_init( &objGTimers[timerid], tid );
    if (periodical) {
        gtimer_start_periodical (&objGTimers[timerid], duration_us, (void *)handler, userdata);
    } else {
        gtimer_start_one_shout (&objGTimers[timerid], duration_us, (void *)handler, userdata);
    }

    ConfigDebugErr = backup_ConfigDebugErr;
}

void GTimerClass::stop(uint32_t timerid)
{
    if (timerid > GTIMER_MAX ) {
        return;
    }

    gtimer_stop(&objGTimers[timerid]);
}

void GTimerClass::reload(uint32_t timerid, uint32_t duration_us)
{
    if (timerid > GTIMER_MAX ) {
        return;
    }

    gtimer_reload(&objGTimers[timerid], duration_us);
}

uint64_t GTimerClass::read_us(uint32_t timerid)
{
    if (timerid > GTIMER_MAX ) {
        return 0;
    }

    return gtimer_read_us(&objGTimers[timerid]);
}

GTimerClass GTimer;

