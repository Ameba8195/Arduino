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
#include "variant.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "PinNames.h"

void __libc_init_array(void);

/*
 * Pins descriptions
 */
PinDescription g_APinDescription[TOTAL_GPIO_PIN_NUM]=
{

  {PA_6, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D0  :      UART0_RX
  {PA_7, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D1  :      UART0_TX
  {PA_5, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D2  :
  {PD_4, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D3  : PWM0 UART2_RX
  {PD_5, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D4  : PWM1
  {PA_4, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D5  :      UART2_TX
  {PA_3, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D6  :
  {PA_2, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D7  :
  {PB_4, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D8  : PWM0
  {PB_5, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D9  : PWM1
  {PC_0, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D10 : PWM0 UART0_RX
  {PC_2, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D11 : PWM2
  {PC_3, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D12 : PWM3 UART0_TX
  {PC_1, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D13 : PWM1
  {PB_3, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D14 : 
  {PB_2, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D15 :
  {PA_1, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D16 :
  {PA_0, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D17 :      UART2_RX
  {PE_5, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}  //D18 :

};

#ifdef __cplusplus
} // extern C
#endif

void serialEvent() __attribute__((weak));
bool Serial_available() __attribute__((weak));

// ----------------------------------------------------------------------------

void serialEventRun(void)
{
    if (Serial_available && serialEvent && Serial_available()) serialEvent();

//  if (Serial1.available()) serialEvent1();
//  if (Serial2.available()) serialEvent2();
//  if (Serial3.available()) serialEvent3();

}

void init( void )
{
    // Initialize C library
    __libc_init_array();
}

