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



/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "Arduino.h"
#include <stdarg.h>
#ifdef __cplusplus
#include "LOGUARTClass.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

#define portOutputRegister(P) ( (volatile uint32_t *)( 0x40001000 + (P) * 0x0C ) )
#define portInputRegister(P)  ( (volatile uint32_t *)( 0x40001050 + (P) * 4 ) )

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

extern LOGUARTClass Serial;
//extern USARTClass Serial1;
//extern USARTClass Serial2;
//extern USARTClass Serial3;

//#include "UARTClass1.h"
//extern UARTClass1 Serial1;
#endif

#endif /* _VARIANT_ARDUINO_AMEBA_ */

