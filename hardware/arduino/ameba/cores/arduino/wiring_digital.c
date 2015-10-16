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

#include "PinNames.h"
#include "objects.h"
#include "hal_gpio.h"
#include "section_config.h"


static const u8 _GPIO_SWPORT_DR_TBL[] = {
    GPIO_PORTA_DR,
    GPIO_PORTB_DR,
    GPIO_PORTC_DR
};

static const u8 _GPIO_EXT_PORT_TBL[] = {
    GPIO_EXT_PORTA,
    GPIO_EXT_PORTB,
    GPIO_EXT_PORTC
};


typedef struct {
	gpio_t 	sGpio_t;
	u8		pin_num;
	u8		port_num;
	u8 		port_write;
	u8 		port_read;
} gpio_pin_t;


IMAGE2_DATA_SECTION
gpio_pin_t gpio_pin_struct[14];


IMAGE2_TEXT_SECTION
void pinMode( uint32_t ulPin, uint32_t ulMode )
{
	//NeoJou
	PinName pinname;

	gpio_t *pGpio_t;
	gpio_pin_t *pGpio_pin_t;
	HAL_GPIO_PIN *pHal_gpio_pin;


	//DiagPrintf("[pinMode] ulPin=%d, ulMode=%d\r\n", ulPin, ulMode);
	if ( ulPin < 0 || ulPin > 18 ) return;

	pGpio_pin_t = &gpio_pin_struct[ulPin];
	pGpio_t = &gpio_pin_struct[ulPin].sGpio_t;
	pHal_gpio_pin = &pGpio_t->hal_pin;

	if ( g_APinDescription[ulPin].ulPinType == PIO_GPIO && 
		 g_APinDescription[ulPin].ulPinMode == ulMode ) {
		 return;
	}

	if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO ) {
		pinname = g_APinDescription[ulPin].pinname;
		//pinname = digital_pinMap[ulPin];
		gpio_init(pGpio_t, pinname);
		g_APinDescription[ulPin].ulPinType = PIO_GPIO;
	}


	switch ( ulMode )
	{
		case INPUT:
			gpio_dir(pGpio_t, PIN_INPUT);	// Direction: Input
			gpio_mode(pGpio_t, PullNone); 	// No pull
			break ;

		case INPUT_PULLUP:
			gpio_dir(pGpio_t, PIN_INPUT);	// Direction: Input
			gpio_mode(pGpio_t, PullUp); 	// Pull Up
			break ;

		case OUTPUT:
			gpio_dir(pGpio_t, PIN_OUTPUT);	// Direction: Output
			gpio_mode(pGpio_t, PullNone); 	// No pull
		break ;

		case OPEN_DRAIN:
			gpio_dir(pGpio_t, PIN_OUTPUT);	// Direction: Output
			gpio_mode(pGpio_t, OpenDrain); 	// No pull

		break;

		default:
		break ;
	}
	
	g_APinDescription[ulPin].ulPinMode = ulMode;

	//
	
	pGpio_pin_t->pin_num = HAL_GPIO_GET_PIN_BY_NAME(pHal_gpio_pin->pin_name);
	pGpio_pin_t->port_num = HAL_GPIO_GET_PORT_BY_NAME(pHal_gpio_pin->pin_name);
	pGpio_pin_t->port_write = _GPIO_SWPORT_DR_TBL[pGpio_pin_t->port_num];
	pGpio_pin_t->port_read = _GPIO_EXT_PORT_TBL[pGpio_pin_t->port_num];

	DiagPrintf(" %s : pin_num = %d \r\n", __FUNCTION__, pGpio_pin_t->pin_num);
}


IMAGE2_TEXT_SECTION
void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{

	gpio_pin_t *pGpio_pin_t;
	gpio_t *pGpio_t;
    u32 RegValue;

	if ( ulPin < 0 || ulPin > 18 ) return;

	/* Handle */
	if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
	{
	  return ;
	}

	pGpio_pin_t = &gpio_pin_struct[ulPin];

	pGpio_t = &gpio_pin_struct[ulPin].sGpio_t;

	#if 0
	gpio_write(pGpio_t, ulVal);
	#else
	{
	    u8 port_num;
	    u8 pin_num;
		u8 port_write;

	    port_num = pGpio_pin_t->port_num;
	    pin_num = pGpio_pin_t->pin_num;
		port_write = pGpio_pin_t->port_write;
	    
		RegValue =	HAL_READ32(GPIO_REG_BASE, port_write);
		RegValue &= ~(1 << pin_num);
		RegValue |= ((ulVal&0x01)<< pin_num);
		HAL_WRITE32(GPIO_REG_BASE, port_write, RegValue);			
	}
	#endif

}

IMAGE2_TEXT_SECTION
int digitalRead( uint32_t ulPin )
{
	gpio_pin_t *pGpio_pin_t;	
	gpio_t *pGpio_t;	
    u32 RegValue;
	
	if ( ulPin < 0 || ulPin > 18 ) return;

	/* Handle */
	if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
	{
	  return ;
	}

	pGpio_pin_t = &gpio_pin_struct[ulPin];

	pGpio_t = &gpio_pin_struct[ulPin].sGpio_t;

	//NeoJou
	#if 1
	{
	    u8 port_num;
	    u8 pin_num;
		u8 port_read;
		int pin_status;

	    port_num = pGpio_pin_t->port_num;
	    pin_num = pGpio_pin_t->pin_num;
		port_read = pGpio_pin_t->port_read;
		

    	RegValue =  HAL_READ32(GPIO_REG_BASE, port_read);
	    if (RegValue & (1<<pin_num)) {
    	    pin_status = GPIO_PIN_HIGH;
    	}
    	else {
        	pin_status = GPIO_PIN_LOW;
    	}

		return pin_status;
	}
	#else
	return gpio_read(pGpio_t);
	#endif
	
	
}

void digital_isr( uint32_t ulPin, void* handler, void* data)
{
	gpio_pin_t *pGpio_pin_t;
	gpio_t *pGpio_t;
    u32 RegValue;
	HAL_GPIO_PIN *pHal_gpio_pin;

	if ( ulPin < 0 || ulPin > 18 ) return;

	/* Handle */
	if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
	{
		DiagPrintf(" %s : not GPIO pin \r\n", __FUNCTION__);
	  return ;
	}

	pGpio_pin_t = &gpio_pin_struct[ulPin];

	pGpio_t = &gpio_pin_struct[ulPin].sGpio_t;

	pGpio_t->hal_pin.pin_mode = INT_FALLING;


    HAL_GPIO_Irq_Init(&(pGpio_t->hal_pin));
    HAL_GPIO_UserRegIrq(&(pGpio_t->hal_pin), (void*)handler, (void*) data);
	
    HAL_GPIO_IntCtrl(&(pGpio_t->hal_pin), _TRUE);

	
    HAL_GPIO_UnMaskIrq(&(pGpio_t->hal_pin));

}

u8 gpio_get_pin_num(uint32_t ulPin)
{
	gpio_pin_t *pGpio_pin_t;	
	gpio_t *pGpio_t;	

	u8 pin_num;

	if ( ulPin < 0 || ulPin > 18 ) return;

	/* Handle */
	if ( g_APinDescription[ulPin].ulPinType != PIO_GPIO )
	{
		DiagPrintf(" %s : not GPIO pin \r\n", __FUNCTION__);
	  return ;
	}

	pGpio_pin_t = &gpio_pin_struct[ulPin];

	pin_num = pGpio_pin_t->pin_num;

	DiagPrintf("%s : pin_num=%d \r\n", __FUNCTION__, pin_num);
	return pin_num;
} 

#ifdef __cplusplus
}
#endif

