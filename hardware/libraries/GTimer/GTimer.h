#ifndef _GTIMER_H_
#define _GTIMER_H_

#include <inttypes.h>

class GTimerClass {
public:
    void begin(uint32_t timerid, uint32_t duration_us, void (*handler)(uint32_t), bool periodical=true, uint32_t userdata=0);
    void stop(uint32_t timerid);
    void reload(uint32_t timerid, uint32_t duration_us);
    uint64_t read_us(uint32_t timerid);
};

extern GTimerClass GTimer;

#endif