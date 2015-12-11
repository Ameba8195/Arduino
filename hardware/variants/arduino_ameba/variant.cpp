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

void __libc_init_array(void);

/*
 * Pins descriptions
 */
PinDescription g_APinDescription[TOTAL_GPIO_PIN_NUM]=
{
  
  {PA_6, NOT_INITIAL, NOT_INITIAL}, //D0
  {PA_7, NOT_INITIAL, NOT_INITIAL}, //D1
  {PA_5, NOT_INITIAL, NOT_INITIAL}, //D2
  {PD_4, NOT_INITIAL, NOT_INITIAL}, //D3  : PWM1
  {PD_5, NOT_INITIAL, NOT_INITIAL}, //D4  : PWM2
  {PA_4, NOT_INITIAL, NOT_INITIAL}, //D5
  {PA_3, NOT_INITIAL, NOT_INITIAL}, //D6
  {PA_2, NOT_INITIAL, NOT_INITIAL}, //D7
  {PB_4, NOT_INITIAL, NOT_INITIAL}, //D8  : PWM0
  {PB_5, NOT_INITIAL, NOT_INITIAL}, //D9  : PWM1
  {PC_0, NOT_INITIAL, NOT_INITIAL}, //D10 : PWM0
  {PC_2, NOT_INITIAL, NOT_INITIAL}, //D11 : PWM2
  {PC_3, NOT_INITIAL, NOT_INITIAL}, //D12 : PWM3
  {PC_1, NOT_INITIAL, NOT_INITIAL}, //D13 : PWM1
  {PB_3, NOT_INITIAL, NOT_INITIAL}, //D14 : 
  {PB_2, NOT_INITIAL, NOT_INITIAL}, //D15 :
  {PA_1, NOT_INITIAL, NOT_INITIAL}, //D16 :
  {PA_0, NOT_INITIAL, NOT_INITIAL}, //D17 :
  {PE_5, NOT_INITIAL, NOT_INITIAL}  //D18 :

};

analogin_t   adc1;
analogin_t   adc2;
analogin_t   adc3;

#ifdef __cplusplus
} // extern C
#endif


// LogUart
static RingBuffer rx_buffer0;

LOGUARTClass Serial(UART_LOG_IRQ, &rx_buffer0);

void serialEvent() __attribute__((weak));
void serialEvent() { }

// ----------------------------------------------------------------------------

void serialEventRun(void)
{

  //if (Serial.available()) serialEvent();
//  if (Serial1.available()) serialEvent1();
//  if (Serial2.available()) serialEvent2();
//  if (Serial3.available()) serialEvent3();

}

void init( void )
{
    // Initialize C library
    __libc_init_array();
}

