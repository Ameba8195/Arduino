#include "PowerManagement.h"

#ifdef __cplusplus
extern "C" {

#include "freertos_pmu.h"
#include "sys_api.h"

#include "wiring_digital.h"

}
#endif

#include "variant.h"

bool PowerManagementClass::reservePLL = true;

void PowerManagementClass::setPllReserved(bool reserve) {
    set_pll_reserved(reserve);
}

void PowerManagementClass::sleep() {
    pinMode(18, INPUT_PULLUP);
    if (digitalRead(18) == 1) {
        release_wakelock(WAKELOCK_OS);
    }
}

void PowerManagementClass::active() {
    acquire_wakelock(WAKELOCK_OS);
}

void PowerManagementClass::deepsleep(uint32_t duration_ms) {
    pinMode(18, INPUT_PULLUP);
    if (digitalRead(18) == 1) {
        deepsleep_ex(DSLEEP_WAKEUP_BY_TIMER, duration_ms);
    }
}

void PowerManagementClass::softReset() {
    sys_reset();
}

PowerManagementClass PowerManagement;

