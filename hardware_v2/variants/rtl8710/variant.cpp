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

  {PA_2, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D0  : GPIOA_2, UART2_RTS
  {PC_4, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D1  : GPIOC_4_INT, I2C1_SDA
  {PA_5, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D2  : GPIOA_5
  {PA_1, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D3  : GPIOA_1_INT, UART2_CTS
  {PA_0, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D4  : GPIOA_0_INT, UART2_IN
  {PA_3, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D5  : GPIOA_3
  {PE_0, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D6  : GPIOE_0, UART0_OUT, JTAG_TRST
  {PA_4, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D7  : GPIOA_4
  {PC_0, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D8  : GPIOC_0, UART0_IN, SPI0_CS0, PWM0
  {PC_3, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D9  : GPIOC_3_INT, UART0_OUT, SPI0_MISO, PWM3
  {PC_1, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ | PIO_PWM, NOT_INITIAL}, //D10 : GPIOC_1_INT, UART0_CTS, SPI0_CLK, PWM1
  {PB_1, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D11 : GPIOB_1, UART_LOG_IN
  {PB_0, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D12 : GPIOB_0, UART_LOG_OUT
  {PC_2, NOT_INITIAL, PIO_GPIO                | PIO_PWM, NOT_INITIAL}, //D13 : GPIOC_2, UART0_RTS, SPI0_MOSI, PWM2
  {PC_5, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D14 : GPIOC_5_INT, I2C1_SCL
  {PE_4, NOT_INITIAL, PIO_GPIO                         , NOT_INITIAL}, //D15 : GPIOE_4, JTAG_CLK
  {PE_1, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D16 : GPIOE_1_INT, UART0_RTS, JTAG_TDI
  {PE_3, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}, //D17 : GPIOE_3_INT, UART0_IN, JTAG_TMS
  {PE_2, NOT_INITIAL, PIO_GPIO | PIO_GPIO_IRQ          , NOT_INITIAL}  //D18 : GPIOE_2_INT, UART0_CTS, JTAG_TDO

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

