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

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio_api.h"

gpio_t gpio_pin_struct[TOTAL_GPIO_PIN_NUM];

void pinMode( uint32_t ulPin, uint32_t ulMode )
{
    gpio_t *pGpio_t;

    if ( ulPin < 0 || ulPin > TOTAL_GPIO_PIN_NUM )
    {
        return;
    }

    if ( g_APinDescription[ulPin].ulPinType == PIO_GPIO )
    {
        return;
    }

    pGpio_t = &gpio_pin_struct[ulPin];

    if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO ) {
        gpio_init( pGpio_t, g_APinDescription[ulPin].pinname );
        g_APinDescription[ulPin].ulPinType = PIO_GPIO;
    }

    switch ( ulMode )
    {
        case INPUT:
            gpio_dir( pGpio_t, PIN_INPUT );
            gpio_mode( pGpio_t, PullDown );
            break;

        case INPUT_PULLNONE:
            gpio_dir( pGpio_t, PIN_INPUT );
            gpio_mode( pGpio_t, PullNone );
            break;

        case INPUT_PULLUP:
            gpio_dir( pGpio_t, PIN_INPUT );
            gpio_mode( pGpio_t, PullUp );
            break;

        case OUTPUT:
            gpio_dir( pGpio_t, PIN_OUTPUT );
            gpio_mode( pGpio_t, PullNone );
            break;

        case OUTPUT_OPENDRAIN:
            gpio_dir( pGpio_t, PIN_OUTPUT );
            gpio_mode( pGpio_t, OpenDrain );
            break;

        default:
            break ;
    }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
    gpio_t *pGpio_t;

    if ( ulPin < 0 || ulPin > TOTAL_GPIO_PIN_NUM )
    {
        return;
    }

    if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
    {
        return;
    }

    pGpio_t = &gpio_pin_struct[ulPin];

    gpio_write( pGpio_t, ulVal );
}

int digitalRead( uint32_t ulPin )
{
    gpio_t *pGpio_t;
    int pin_status;
    
    if ( ulPin < 0 || ulPin > TOTAL_GPIO_PIN_NUM )
    {
        return -1;
    }

    if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
    {
        return -1;
    }

    pGpio_t = &gpio_pin_struct[ulPin];

    pin_status = gpio_read( pGpio_t );

    return pin_status;
}

void digitalChangeDir( uint32_t ulPin, uint8_t direction)
{
	gpio_t *pGpio_t;
    u32 RegValue;

	if ( ulPin < 0 || ulPin > TOTAL_GPIO_PIN_NUM )
    {
        return;
    }

	if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
	{
	  return ;
	}

    pGpio_t = &gpio_pin_struct[ulPin];

    gpio_dir( pGpio_t, direction );
}

#ifdef __cplusplus
}
#endif

