#ifndef __FREERTOS_PMU_H_
#define __FREERTOS_PMU_H_

#ifdef CONFIG_PLATFORM_8195A
#include "sleep_ex_api.h"
#endif

#ifndef BIT
#define BIT(n)                   (1<<n)
#endif

#define DEFAULT_WAKEUP_EVENT (SLEEP_WAKEUP_BY_STIMER | SLEEP_WAKEUP_BY_GTIMER | SLEEP_WAKEUP_BY_GPIO_INT | SLEEP_WAKEUP_BY_WLAN)

#ifdef CONFIG_PLATFORM_8195A
typedef enum PMU_DEVICE {

    PMU_OS = 0,
    PMU_WLAN_DEVICE = 1,
    PMU_LOGUART_DEVICE = 2,
    PMU_SDIO_DEVICE = 3,

    PMU_DEV_USERT_BASE= 16,

    PMU_MAX = 31

} PMU_DEVICE;
#endif

#ifdef CONFIG_PLATFORM_8711B
typedef enum PMU_DEVICE {
    PMU_OS = 0,
    PMU_WLAN_DEVICE = 1,
    PMU_LOGUART_DEVICE = 2,
    PMU_SDIO_DEVICE = 3,
    PMU_UART0_DEVICE = 4,
    PMU_UART1_DEVICE = 5,
    PMU_I2C0_DEVICE = 6,
    PMU_I2C1_DEVICE = 7,
    PMU_USOC_DEVICE = 8,
    PMU_DONGLE_DEVICE = 9,
    PMU_RTC_DEVICE = 10,
    PMU_CONSOLE_DEVICE = 11,

    PMU_DEV_USERT_BASE= 16,

    PMU_MAX = 31
} PMU_DEVICE;
#endif

// default locked by OS and not to sleep until OS release wakelock in somewhere
#define DEFAULT_WAKELOCK         (BIT(PMU_OS))

typedef uint32_t (*PSM_HOOK_FUN)( unsigned int, void* param_ptr );

/** Acquire wakelock
 *
 *  A wakelock is a 32-bit map. Each module own 1 bit in this bit map.
 *  FreeRTOS tickless reference the wakelock and decide that if it can or cannot enter sleep state.
 *  If any module acquire and hold a bit in wakelock, then the whole system won't enter sleep state.
 *
 *  If wakelock is not equals to 0, then the system won't enter sleep.
 *
 *  @param lock_id        : The bit which is attempt to add into wakelock
 */
void pmu_acquire_wakelock(uint32_t lock_id);

/** Release wakelock
 *
 *  If wakelock equals to 0, then the system may enter sleep state if it is in idle state.
 *
 *  @param lock_id        : The bit which is attempt to remove from wakelock
 */
void pmu_release_wakelock(uint32_t lock_id);

/** Get current wakelock bit map value
 *
 *  @return               : the current wakelock bit map value
 */
uint32_t pmu_get_wakelock_status();

#if (configGENERATE_RUN_TIME_STATS == 1)

/** enable to keep wakelock stats
 *
 */
void pmu_enable_wakelock_stats( unsigned char enable );

/** Get text report that contain the statics of wakelock holding time
 *
 *  Each time a module acquries or releases wakelock, a holding time is calculated and sum up to a table.
 *  It is for debug that which module is power saving killer.
 *
 *  @param pcWriteBuffer  : The char buffer that contain the report
 */
void pmu_get_wakelock_hold_stats( char *pcWriteBuffer );

/** Recalculate the wakelock statics
 *
 *  By default the wakelock statics is calculated from system boot up.
 *  If we want to debug power saving killer from a specified timestamp, we can reset the statics.
 */
void pmu_clean_wakelock_stat();

#endif

void pmu_add_wakeup_event(uint32_t event);
void pmu_del_wakeup_event(uint32_t event);

void pmu_register_sleep_callback(uint32_t nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void* sleep_param_ptr, PSM_HOOK_FUN wakeup_hook_fun, void* wakeup_param_ptr);
void pmu_unregister_sleep_callback(uint32_t nDeviceId);

#ifdef CONFIG_PLATFORM_8195A
/** Set PLL reserved or not when sleep is called
 *
 *  @param reserve: true for sleep with PLL reserve
 */
void pmu_set_pll_reserved(unsigned char reserve);
#endif

#endif
