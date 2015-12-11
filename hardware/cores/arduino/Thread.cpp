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

extern "C" {
extern _LONG_CALL_ uint32_t DiagPrintf(const char *fmt, ...);
}

Thread::Thread(void (*task)(void const *argument), void *argument,
        osPriority priority, uint32_t stack_size) {

	_thread_arg = argument;
    _thread_def.pthread = task;
    _thread_def.tpriority = priority;
    _thread_def.stacksize = stack_size;
}

int Thread::start() 
{
	_tid = osThreadCreate(&_thread_def, _thread_arg);
	
	if ( _tid == NULL ) {
		DiagPrintf("Thread start failed \n");
		return -1;
	}
	
	return 0;
}

osStatus Thread::terminate() {
    return osThreadTerminate(_tid);
}

osStatus Thread::set_priority(osPriority priority) {
    return osThreadSetPriority(_tid, priority);
}

osPriority Thread::get_priority() {
    return osThreadGetPriority(_tid);
}

int32_t Thread::signal_set(int32_t signals) {
    return osSignalSet(_tid, signals);
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

osEvent Thread::signal_wait(int32_t signals, uint32_t millisec) {
    return osSignalWait(signals, millisec);
}

osStatus Thread::wait(uint32_t millisec) {
    return osDelay(millisec);
}

osStatus Thread::yield() {
    return osThreadYield();
}

osThreadId Thread::gettid() {
    return osThreadGetId();
}

Thread::~Thread() {
    terminate();
}
