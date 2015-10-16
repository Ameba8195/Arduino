/* Tone.cpp

  A Tone Generator Library

  Written by Brett Hagman

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Version Modified By Date     Comments
------- ----------- -------- --------
0001    B Hagman    09/08/02 Initial coding
0002    B Hagman    09/08/18 Multiple pins
0003    B Hagman    09/08/18 Moved initialization from constructor to begin()
0004    B Hagman    09/09/26 Fixed problems with ATmega8
0005    B Hagman    09/11/23 Scanned prescalars for best fit on 8 bit timers
                    09/11/25 Changed pin toggle method to XOR
                    09/11/25 Fixed timer0 from being excluded
0006    D Mellis    09/12/29 Replaced objects with functions
0007    M Sproul    10/08/29 Changed #ifdefs from cpu to register
0008    S Kanemoto  12/06/22 Fixed for Leonardo by @maris_HY
*************************************************/

#include "Arduino.h"

#include "DAC1.h"




#define MAX_TONE_BUFFER_SIZE (8192)
static uint16_t buffer[MAX_TONE_BUFFER_SIZE];
static unsigned int buffer_size = 0;
static unsigned int buffer_page=0;

#define PI        (3.141592653589793238462)
#define AMPLITUDE (1.0)    // x * 3.3V
#define PHASE     (PI * 1) // 2*pi is one period
#define RANGE     (4096/2) // 12 bits DAC
#define OFFSET    (4096/2) // 12 bits DAC


// Create the sinewave buffer
void Tone1::calculate_sinewave(uint16_t *pBuffer, unsigned int buf_size)
{
  double amplitude = (double)(volume)/100.0;
  for (int i = 0; i < buf_size; i++) {
     double rads = (2*PI * i)/buf_size; // Convert degree in radian
     double val;

	 val = ((double)(RANGE*amplitude) * (cos(rads + PHASE))) + (double)(OFFSET);
     pBuffer[i] = (uint16_t)val;
     //rtl_printf("0x%x ", buffer[i]);
  }
}

// Create the sinewave buffer
void Tone1::calculate_zero(uint16_t *pBuffer, int s)
{
  for (int i = 0; i < s; i++) {
     pBuffer[i] = 0x800;
  }
}


// frequency (in hertz) and duration (in milliseconds).

void Tone1::tone(unsigned int frequency, unsigned long duration)
{
	unsigned int repeat;

    if ( frequency <= 0 ) {
		noTone(duration);
		return;
	} 
	
	buffer_size = 250000 / frequency ; // 250K 
	if ( buffer_size > MAX_TONE_BUFFER_SIZE ) buffer_size = MAX_TONE_BUFFER_SIZE;
	if ( buffer_size <= 0 ) buffer_size = 250;
	calculate_sinewave(buffer, buffer_size);
	
	repeat = 250*duration/buffer_size; // duration = 1000/time ; 10us
	if ( repeat < 1 ) repeat = 1;

	DAC0.send16_repeat((uint16_t*)buffer, buffer_size, repeat);
	
}



void Tone1::noTone(unsigned long duration)
{
	unsigned int repeat;

	buffer_size = 250;	
	calculate_zero(buffer, buffer_size);

	repeat = 250*duration/buffer_size; // duration = 1000/time ; 10us
	if ( repeat < 1 ) repeat = 1;

	DAC0.send16_repeat((uint16_t*)buffer, buffer_size, repeat);
}

