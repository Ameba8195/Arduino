/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Ticker.h"

#include "TimerEvent.h"
#include "FunctionPointer.h"
#include "ticker_api.h"

#include "rt_os_service.h"

void ticker_thread(void *arg)
{
    Ticker *pTicker = (Ticker*)arg;
	while (1) {
		rtw_down_sema(&(pTicker->ticker_thread_sema));
		pTicker->_function.call();
	}
}

void Ticker::ticker_init(void)
{
    rtw_init_sema(&this->ticker_thread_sema, 0);

	tasklet.pthread = (os_pthread)(&ticker_thread);
    tasklet.tpriority = osPriorityRealtime;
    tasklet.stacksize = DEFAULT_STACK_SIZE;
    tasklet.stack_pointer = &this->stack[0];

	_tid = osThreadCreate(&this->tasklet, this);
}

void Ticker::detach() {
    remove();
    _function.attach(0);
}

void Ticker::setup(timestamp_t t) {
    remove();
    _delay = t;
    insert(_delay + ticker_read(_ticker_data));
}

void Ticker::handler() {
    insert(event.timestamp + _delay);	
	rtw_up_sema(&this->ticker_thread_sema);
    //_function.call();
}


