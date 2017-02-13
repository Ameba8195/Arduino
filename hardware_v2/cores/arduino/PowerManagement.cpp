#include "PowerManagement.h"

#ifdef __cplusplus
extern "C" {

#include "freertos_pmu.h"
#include "sys_api.h"

#include "wiring_digital.h"

extern void log_uart_disable_printf(void);

}
#endif

#include "variant.h"

#if defined(BOARD_RTL8195A)
#define SAVE_LOCK_PIN 18
#elif defined(BOARD_RTL8710)
#define SAVE_LOCK_PIN 15
#else
#define SAVE_LOCK_PIN 18
#endif

bool PowerManagementClass::reservePLL = true;
bool PowerManagementClass::safeLockPin = SAVE_LOCK_PIN;

void PowerManagementClass::setPllReserved(bool reserve) {
    pmu_set_pll_reserved(reserve);
}

void PowerManagementClass::sleep() {
    if (!safeLock()) {
        pmu_release_wakelock(BIT(PMU_OS));
    }
}

void PowerManagementClass::active() {
    pmu_acquire_wakelock(BIT(PMU_OS));
}

void PowerManagementClass::deepsleep(uint32_t duration_ms) {
    if (!safeLock()) {
        log_uart_disable_printf();
        deepsleep_ex(DSLEEP_WAKEUP_BY_TIMER, duration_ms);
    }
}

bool PowerManagementClass::safeLock() {
    pinMode(safeLockPin, INPUT_PULLUP);
    return (digitalRead(safeLockPin) == 1) ? false : true;
}

bool PowerManagementClass::setSafeLockPin(int ulPin) {
    if ( ulPin < 0 || ulPin > TOTAL_GPIO_PIN_NUM || (g_APinDescription[ulPin].pinname == NC) ) {
        // Invalid pin
        return false;
    }

    safeLockPin = pin;
    return true;
}

void PowerManagementClass::softReset() {
    sys_reset();
}

PowerManagementClass PowerManagement;

