/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_ARDUINO_AMEBA_
#define _VARIANT_ARDUINO_AMEBA_

#include "Arduino.h"
#include <stdarg.h>
#ifdef __cplusplus
#include "LOGUARTClass.h"
#endif

#define MAIN_THREAD_STACK_SIZE (4096)

#define TOTAL_GPIO_PIN_NUM	(19)
#define TOTAL_PWM_PIN_NUM   ( 4)

#define LED_BUILTIN 13

/* Analog pin mapping */
#define A0 1
#define A1 2
#define A2 3

#define SS 10

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#define portOutputRegister(P) ( (volatile uint32_t *)( 0x40001000 + (P) * 0x0C ) )
#define portInputRegister(P)  ( (volatile uint32_t *)( 0x40001050 + (P) * 4 ) )
#define portModeRegister(P)   ( (volatile uint32_t *)( 0x40001004 + (P) * 0x0C ) )

/*
 * Wait until enter debug mode
 *
 * Check DHCSR(0xE000EDF0) register and hold until bit C_DEBUGEN is set.
 * Use this function along with J-LINK or other debug tool
 **/
extern void wait_for_debug();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

extern LOGUARTClass Serial;

#endif

#endif /* _VARIANT_ARDUINO_AMEBA_ */

