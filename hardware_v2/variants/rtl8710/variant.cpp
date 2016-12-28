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

  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D0  :      UART0_RX
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D1  :      UART0_TX
  {PA_5, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D2  :
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D3  : PWM0 UART2_RX
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D4  : PWM1
  {PA_4, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D5  :      UART2_TX
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D6  :
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D7  :
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D8  : PWM0
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D9  : PWM1
  {PC_0, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D10 : PWM0 UART0_RX
  {PC_2, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D11 : PWM2
  {PC_3, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D12 : PWM3 UART0_TX
  {PC_1, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D13 : PWM1
  {PB_3, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D14 : 
  {PB_2, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D15 :
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}, //D16 :
  {PA_0, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D17 :      UART2_RX
  {NC,   NOT_INITIAL, NOT_INITIAL                      , NOT_INITIAL}  //D18 :

};

void *gpio_pin_struct[TOTAL_GPIO_PIN_NUM] = {NULL};
void *gpio_irq_handler_list[TOTAL_GPIO_PIN_NUM] = {NULL};

/** The heap API in OS layer */
extern int vPortAddHeapRegion(uint8_t *addr, size_t size);

// it should be the last symbol in SRAM in link result 
extern void *__HeapLimit;

// it should be the last symbol in SDRAM in link result
extern void *__sdram_bss_end__;

#ifdef __cplusplus
} // extern C
#endif

void serialEvent() __attribute__((weak));
bool Serial_available() __attribute__((weak));

// ----------------------------------------------------------------------------

void serialEventRun(void)
{
    if (Serial_available && serialEvent && Serial_available()) serialEvent();
}

void init( void )
{
    uint8_t *regionAddr;
    size_t regionSize;

    /* Add non-allocated memory in SRAM into heap */
    regionAddr = (uint8_t *)&__HeapLimit;
    if (0x10070000 > (size_t)(&__HeapLimit)) {
        regionSize = 0x10070000 - (size_t)(&__HeapLimit);
        vPortAddHeapRegion(regionAddr, regionSize);
    }

    // Initialize C library
    __libc_init_array();
}

// ----------------------------------------------------------------------------

void wait_for_debug() {
    while (((CoreDebug->DHCSR) & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0) {
        asm("nop");
    }
    delay(1000);
}

