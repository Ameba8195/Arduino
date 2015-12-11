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

#ifdef __cplusplus
extern "C" {
#endif

#include "us_ticker_api.h"

#ifdef __cplusplus
}
#endif

void ticker_thread(void const *arg)
{
    osEvent evt;
    Ticker *pTicker = (Ticker*)arg;

    osSignalClear(pTicker->getTid(), 0xFFFFFFFF);

    while (1) {
        evt = osSignalWait (0, 0xFFFFFFFF); // wait for any signal with max timeout

        pTicker->_function.call();

        osSignalClear(pTicker->getTid(), 0xFFFFFFFF);
    }
}

void Ticker::ticker_init(void)
{
    osThreadDef(ticker_thread, osPriorityRealtime, 1, configMINIMAL_STACK_SIZE);
    _tid = osThreadCreate (osThread (ticker_thread), this);
}

void Ticker::detach() {
    remove();
    _function.attach(0);
}

void Ticker::setup(timestamp_t t) {
    remove();
    _delay = t;
    insert(_delay + us_ticker_read());
}

void Ticker::handler() {
    insert(event.timestamp + _delay);    
    osSignalSet(_tid, 1);
}

