#ifndef _POWER_MANAGEMENT_H_
#define _POWER_MANAGEMENT_H_

#include <inttypes.h>

class PowerManagementClass {
public:
    static void setPllReserved(bool reserve);
    static void sleep();
    static void active();

    static void deepsleep(uint32_t duration_ms);

    static void softReset();

private:
    static bool reservePLL;
};

extern PowerManagementClass PowerManagement;

#endif