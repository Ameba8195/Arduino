#ifndef _WIRING_WATCHDOG_H_
#define _WIRING_WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Feed watchdog to avoid it barks */
extern void wdt_reset();

/* max timeout is 8s */
extern void wdt_enable(uint32_t timeout_ms);

/* disable watchdog timer */
extern void wdt_disable();

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_WATCHDOG_H_ */

