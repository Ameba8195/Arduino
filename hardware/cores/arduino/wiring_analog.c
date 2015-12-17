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

#include "analogin_api.h"
#include "pwmout_api.h"
#include "gpio_ex_api.h"

analogin_t   adc1;
analogin_t   adc2;
analogin_t   adc3;

static const uint32_t ulPwmPinList[] = { 3, 4, 8, 9, 10, 11, 12, 13 };

pwmout_t pwm_pins[14];

extern gpio_t gpio_pin_struct[];

//
// Arduino
//

static int _readResolution = 10;
static int _writeResolution = 8;

void analogReadResolution(int res) {
	_readResolution = res;
}

void analogWriteResolution(int res) {
	_writeResolution = res;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}

eAnalogReference analog_reference = AR_DEFAULT;

void analogReference(eAnalogReference ulMode)
{
	analog_reference = ulMode;
}

static const float ADC_slope1 = (3.12)/(3410.0-674.0);
static const float ADC_slope2 = (3.3-3.12)/(3454.0-3410.0);

bool g_adc_enabled[] = {
    false, false, false
};

uint32_t analogRead_for_random()
{
    return analogin_read_for_random(&adc1);
}

uint32_t analogRead(uint32_t ulPin)
{
  uint32_t ulValue = 0;
  uint32_t ulChannel;
  uint16_t ret = 0;
  float	   voltage;
  float	   adc_value;

  switch ( ulPin ) {
  	case A0:
        if (g_adc_enabled[0] == false)
        {
            analogin_init(&adc1, AD_1);
            g_adc_enabled[0] = true;
        }
	case A1:
        if (g_adc_enabled[1] == false)
        {
            analogin_init(&adc2, AD_2);
            g_adc_enabled[1] = true;
        }
		ret = analogin_read_u16(&adc2);
		break;
	case A2:
        if (g_adc_enabled[2] == false)
        {
            analogin_init(&adc3, AD_3);
            g_adc_enabled[2] = true;
        }
		ret = analogin_read_u16(&adc3);
		break;
	default:
		DiagPrintf("%s : ulPin %d wrong\n", __FUNCTION__, ulPin);
		return 0;
  }


  ret >>= 4;
  if (ret < 674) {
  	 voltage = 0;
  } else if ( ret > 3410){
     voltage = (float)(ret - 3410)*ADC_slope2 + 3.12;
  } else { 
	 voltage = (float)(ret-674)*ADC_slope1;
  }

  // Arduino analogRead()
  // input : 0~5V
  // 10 bit : 0 ~1023
  ret = round(1023.0*voltage/5.0);
  if ( ret > 1023 ) ret = 1023;
  return ret;
}

void analogOutputInit(void) {

}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint32_t ulPin, uint32_t ulValue) 
{
    uint32_t i;
    bool bValidPin;
	pwmout_t *pObj;

    bValidPin = false;
    for ( i=0; i < ( sizeof(ulPwmPinList) / sizeof(ulPwmPinList[0]) ); i++ ) {
        if (ulPwmPinList[i] == ulPin) {
            bValidPin = true;
            break;
        }
    }
    if (!bValidPin) {
        return;
    }

	/* Handle */
	if ( g_APinDescription[ulPin].ulPinType != PIO_PWM )
	{
	    if ( g_APinDescription[ulPin].ulPinType == PIO_GPIO ) {
            gpio_deinit(&gpio_pin_struct[ulPin], g_APinDescription[ulPin].pinname);
        }
	    pwmout_init(&pwm_pins[ulPin], g_APinDescription[ulPin].pinname);
        pwmout_period_us(&pwm_pins[ulPin], 20000);
		g_APinDescription[ulPin].ulPinType = PIO_PWM;
	}

    pwmout_write(&pwm_pins[ulPin], ulValue / 256.0);
}

#ifdef __cplusplus
}
#endif
