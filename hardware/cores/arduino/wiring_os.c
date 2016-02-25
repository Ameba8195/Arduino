#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "wiring_os.h"
#include "cmsis_os.h"

uint32_t os_thread_create(void (*task)(const void *argument), void *argument, int priority, uint32_t stack_size) {

    osThreadDef_t thread_def;

    thread_def.pthread = task;
    thread_def.tpriority = (osPriority)priority;
    thread_def.stacksize = stack_size;
    thread_def.name = "ARDUINO";

    return (uint32_t)osThreadCreate(&thread_def, argument);
}

uint32_t os_thread_terminate(uint32_t thread_id) {
    return (uint32_t)osThreadTerminate(thread_id);
}

uint32_t os_thread_set_priority(uint32_t thread_id, int priority) {
    return (uint32_t)osThreadSetPriority(thread_id, (osPriority)priority);
}

int os_thread_get_priority(uint32_t thread_id) {
    return (int)osThreadGetPriority(thread_id);
}

int32_t os_signal_set (uint32_t thread_id, int32_t signals) {
    return osSignalSet(thread_id, signals);
}

os_event_t os_signal_wait(int32_t signals, uint32_t millisec) {

    osEvent evt;
    os_event_t ret;

    evt = osSignalWait(signals, millisec);
    ret.status = (uint32_t)evt.status;
    ret.value.signals = evt.value.signals;
    ret.def.message_id = evt.def.message_id;

    return ret;
}

uint32_t os_thread_yield(void) {
    return (uint32_t)osThreadYield();
}

#ifdef __cplusplus
}
#endif