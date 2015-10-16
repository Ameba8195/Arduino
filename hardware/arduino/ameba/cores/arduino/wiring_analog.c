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

//
// NeoJou
//
#include "basic_types.h"

#include "pinmap.h"
#include "hal_pinmux.h"

static const PinMap PinMap_PWM[] = {
    {PB_4,  RTL_PIN_PERI(PWM0, 0, S0), RTL_PIN_FUNC(PWM0, S0)},
    {PB_5,  RTL_PIN_PERI(PWM1, 1, S0), RTL_PIN_FUNC(PWM1, S0)},
    {PB_6,  RTL_PIN_PERI(PWM2, 2, S0), RTL_PIN_FUNC(PWM2, S0)},
    {PB_7,  RTL_PIN_PERI(PWM3, 3, S0), RTL_PIN_FUNC(PWM3, S0)},

    {PC_0,  RTL_PIN_PERI(PWM0, 0, S1), RTL_PIN_FUNC(PWM0, S1)},
    {PC_1,  RTL_PIN_PERI(PWM1, 1, S1), RTL_PIN_FUNC(PWM1, S1)},
    {PC_2,  RTL_PIN_PERI(PWM2, 2, S1), RTL_PIN_FUNC(PWM2, S1)},
    {PC_3,  RTL_PIN_PERI(PWM3, 3, S1), RTL_PIN_FUNC(PWM3, S1)},

    {PD_3,  RTL_PIN_PERI(PWM0, 0, S2), RTL_PIN_FUNC(PWM0, S2)},
    {PD_4,  RTL_PIN_PERI(PWM1, 1, S2), RTL_PIN_FUNC(PWM1, S2)},
    {PD_5,  RTL_PIN_PERI(PWM2, 2, S2), RTL_PIN_FUNC(PWM2, S2)},
    {PD_6,  RTL_PIN_PERI(PWM3, 3, S2), RTL_PIN_FUNC(PWM3, S2)},

    {PE_0,  RTL_PIN_PERI(PWM0, 0, S3), RTL_PIN_FUNC(PWM0, S3)},
    {PE_1,  RTL_PIN_PERI(PWM1, 1, S3), RTL_PIN_FUNC(PWM1, S3)},
    {PE_2,  RTL_PIN_PERI(PWM2, 2, S3), RTL_PIN_FUNC(PWM2, S3)},
    {PE_3,  RTL_PIN_PERI(PWM3, 3, S3), RTL_PIN_FUNC(PWM3, S3)},

    {NC,    NC,     0}
};



typedef struct pwmout_s pwmout_t;

static pwmout_t pwm_pins[14];

static void pwmout_init         (pwmout_t* obj, PinName pin);



void pwmout_init(pwmout_t* obj, PinName pin) 
{
    uint32_t peripheral;
    u32 pwm_idx;
    u32 pin_sel;

    
    // Get the peripheral name from the pin and assign it to the object
    peripheral = pinmap_peripheral(pin, PinMap_PWM);

    if ( peripheral == NC ) {
        return;
    }

    pwm_idx = RTL_GET_PERI_IDX(peripheral);
    pin_sel = RTL_GET_PERI_SEL(peripheral);

    obj->pwm_idx = pwm_idx;
    obj->pin_sel = pin_sel;
    obj->period = 0;
    obj->pulse = 0;
    HAL_Pwm_Init(pwm_idx, pin_sel);
	obj->period = 20000;
    HAL_Pwm_SetDuty(obj->pwm_idx, obj->period, obj->pulse);
    HAL_Pwm_Enable(pwm_idx);
}




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
uint32_t analogRead_for_random()
{
    return analogin_read_for_random(&adc1);
}

//NeoJou
// analogRead : ulPin : only for ADC using; 
uint32_t analogRead(uint32_t ulPin)
{
  uint32_t ulValue = 0;
  uint32_t ulChannel;
  uint16_t ret = 0;
  float	   voltage;
  float	   adc_value;

  switch ( ulPin ) {
  	case 0:
	case 1:
		ret = analogin_read_u16(&adc2);
		break;
	case 2:
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


static uint8_t PWMEnabled = 0;
//static uint8_t pinEnabled[PINS_COUNT];

void analogOutputInit(void) {
	uint8_t i;
//	for (i=0; i<PINS_COUNT; i++)
//		pinEnabled[i] = 0;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint32_t ulPin, uint32_t ulValue) 
{

	pwmout_t *pObj;

	if ( ulPin < 0 || ulPin > 13 ) return;

	/* Handle */
	if ( g_APinDescription[ulPin].ulPinType != PIO_PWM )
	{
		pwmout_init(&pwm_pins[ulPin], g_APinDescription[ulPin].pinname);
		g_APinDescription[ulPin].ulPinType = PIO_PWM;
	}

	pObj = &pwm_pins[ulPin];
    pObj->pulse = pObj->period * ulValue / 256;
    HAL_Pwm_SetDuty(pObj->pwm_idx, pObj->period, pObj->pulse);


}

#ifdef __cplusplus
}
#endif
