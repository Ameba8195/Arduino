/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "Thread.h"

#ifdef __cplusplus
extern "C" {
#include "cmsis_os.h"
}
#endif

Thread::Thread(void (*task)(void const *argument), void *argument,
        int priority, uint32_t stack_size) {

	_thread_arg = argument;
    _thread_def = (osThreadDef_t *) malloc( sizeof(osThreadDef_t) );
    ((osThreadDef_t *)_thread_def)->pthread = task;
    ((osThreadDef_t *)_thread_def)->tpriority = (osPriority)priority;
    ((osThreadDef_t *)_thread_def)->stacksize = stack_size;
    ((osThreadDef_t *)_thread_def)->name = "ARDUINO";
}

int Thread::start() 
{
	_tid = (uint32_t)osThreadCreate((osThreadDef_t *)_thread_def, _thread_arg);
	
	if ( _tid == NULL ) {
		return -1;
	}
	
	return 0;
}

uint32_t Thread::terminate() {
    delete(_thread_def);
    return osThreadTerminate((osThreadId)_tid);
}

uint32_t Thread::set_priority(int priority) {
    return osThreadSetPriority((osThreadId)_tid, (osPriority)priority);
}

int Thread::get_priority() {
    return osThreadGetPriority((osThreadId)_tid);
}

int32_t Thread::signal_set(int32_t signals) {
    return osSignalSet((osThreadId)_tid, signals);
}

// not support in cmsis_os version 1.02
#if 0
Thread::State Thread::get_state() {
#ifndef __MBED_CMSIS_RTOS_CA9
    return ((State)_thread_def.tcb.state);
#else
    uint8_t status;
    status = osThreadGetState(_tid);
    return ((State)status);
#endif
}
#endif

uint32_t Thread::signal_wait(int32_t signals, uint32_t millisec) {
    osEvent evt = osSignalWait(signals, millisec);
    return (uint32_t)evt.status;
}

uint32_t Thread::wait(uint32_t millisec) {
    return osDelay(millisec);
}

uint32_t Thread::yield() {
    return osThreadYield();
}

uint32_t Thread::gettid() {
    return (uint32_t)osThreadGetId();
}

Thread::~Thread() {
    terminate();
}
