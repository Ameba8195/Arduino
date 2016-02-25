#ifndef _WIRING_OS_H_
#define _WIRING_OS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// redefined enum osStatus in cmsis_os.h
#define OS_OK                     0x00
#define OS_EVENT_SIGNAL           0x08
#define OS_EVENT_MESSAGE          0x10
#define OS_EVENT_MAIL             0x20
#define OS_EVENT_TIMEOUT          0x40
#define OS_ERROR_PARAMETER        0x80
#define OS_ERROR_RESOURCE         0x81
#define OS_ERROR_TIMEOUT_RESOURCE 0xC1
#define OS_ERROR_ISR              0x82
#define OS_ERROR_ISR_RECURSIVE    0x83
#define OS_ERROR_PRIORITY         0x84
#define OS_ERROR_NO_MEMORY        0x85
#define OS_ERROR_VALUE            0x86
#define OS_ERROR_OS               0xFF

// redefine enum osPriority
#define OS_PRIORITY_IDLE           (-3)
#define OS_PRIORITY_LOW            (-2)
#define OS_PRIORITY_BELOW_NORMAL   (-1)
#define OS_PRIORITY_NORMAL         ( 0)
#define OS_PRIORITY_ABOVENORMAL    (+1)
#define OS_PRIORITY_HIGH           (+2)
#define OS_PRIORITY_REALTIME       (+3)

#ifndef DEFAULT_STACK_SIZE
#define DEFAULT_STACK_SIZE 512
#endif

// redefine struct osEvent in cmsis_os.h
typedef struct {
  uint32_t                 status;     ///< status code: event or error information
  union  {
    uint32_t                    v;     ///< message as 32-bit value
    void                       *p;     ///< message or mail as void pointer
    int32_t               signals;     ///< signal flags
  } value;                             ///< event value
  union  {
    void                 *mail_id;     ///< mail id obtained by \ref osMailCreate
    void              *message_id;     ///< message id obtained by \ref osMessageCreate
  } def;                               ///< event definition
} os_event_t;

extern uint32_t os_thread_create(void (*task)(const void *argument), void *argument, int priority, uint32_t stack_size);

extern uint32_t os_thread_terminate(uint32_t thread_id);

extern uint32_t os_thread_set_priority(uint32_t thread_id, int priority);

extern int os_thread_get_priority(uint32_t thread_id);

extern int32_t os_signal_set (uint32_t thread_id, int32_t signals);

extern os_event_t os_signal_wait(int32_t signals, uint32_t millisec);

extern uint32_t os_thread_yield(void);

#ifdef __cplusplus
}
#endif

#endif
