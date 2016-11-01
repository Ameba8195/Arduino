#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "wdt_api.h"

void wdt_reset() {
    watchdog_refresh();
}

void wdt_enable(uint32_t timeout_ms) {
    if (timeout_ms > 8000) {
        timeout_ms = 8000;
    }
    watchdog_init(timeout_ms);
    watchdog_start();
}

void wdt_disable() {
    watchdog_stop();
}

#ifdef __cplusplus
} // extern "C"
#endif
