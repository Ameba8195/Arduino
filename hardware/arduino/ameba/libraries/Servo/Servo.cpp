/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/


#include <Arduino.h>
#include <Servo.h>

#include "rt_os_service.h"
#include "Ticker.h"


static servo_t servos[MAX_SERVOS];                          // static array of servo structures

uint8_t ServoCount = 0;                                     // the total number of attached servos

// convenience macros
#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min * 4)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max * 4)  // maximum value in uS for this servo

/************ static functions common to all instances ***********************/

static uint8_t isThreadCreated = false;
static Ticker flipper;

static const int delta_time = 28;
static osThreadDef_t g_servo_task;

void servo_handler(void) 
{
	while (1) {
		unsigned int max_count=0; 
		for (int i=0; i<ServoCount; i++) {
			if ( servos[i].Pin.isActive == true ) {
				if ( servos[i].us > 0 ) {				
					digitalWrite(servos[i].Pin.nbr, HIGH);
					if ( servos[i].us > max_count ) max_count = servos[i].us;
				}
			}
		}

		uint32_t start=0, current=0;
		uint32_t spend=0;
		start = us_ticker_read();
		uint8_t loop = true;
		while ( loop ) {
			current = us_ticker_read();
			if ( current >= start )
				spend = current- start;
			else 
				spend = 0xFFFFFFFF - start + current;
			for (int i=0; i<ServoCount; i++) {
				if ( servos[i].Pin.isActive == true ) {
					if ( spend >= servos[i].us - delta_time ) {				
						digitalWrite(servos[i].Pin.nbr, LOW);
						loop = false;
					}
				}
			}
		}

		delay( (20000-2500*ServoCount)/1000 );
	}
}



/****************** end of static functions ******************************/

Servo::Servo()
{
  if (ServoCount < MAX_SERVOS) {
    this->servoIndex = ServoCount++;                    // assign a servo index to this instance
    servos[this->servoIndex].us = DEFAULT_PULSE_WIDTH;   // store default values
  } else {
    this->servoIndex = INVALID_SERVO;  // too many servos
  }


}

uint8_t Servo::attach(int pin)
{
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t Servo::attach(int pin, int min, int max)
{
  if (this->servoIndex < MAX_SERVOS) {
    pinMode(pin, OUTPUT);                                   // set servo pin to output
    servos[this->servoIndex].Pin.nbr = pin;
    // todo min/max check: abs(min - MIN_PULSE_WIDTH) /4 < 128
    this->min  = (MIN_PULSE_WIDTH - min)/4; //resolution of min/max is 4 uS
    this->max  = (MAX_PULSE_WIDTH - max)/4;
    servos[this->servoIndex].Pin.isActive = true;  // this must be set after the check for isTimerActive
  }
  return this->servoIndex;
}

void Servo::detach()
{
  servos[this->servoIndex].Pin.isActive = false;
}

void Servo::write(int value)
{
	if (!isThreadCreated) {		
		rtw_create_thread(&g_servo_task, (os_pthread)servo_handler,  NULL, osPriorityNormal, DEFAULT_STACK_SIZE);
		//flipper.attach(servo_handler, 0.02);
		isThreadCreated = true;
	}
	
  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
  if (value < MIN_PULSE_WIDTH)
  {
    if (value < 0)
      value = 0;
    else if (value > 180)
      value = 180;

    value = map(value, 0, 180, SERVO_MIN(), SERVO_MAX());
  }
  writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
  // calculate and store the values for the given channel
  byte channel = this->servoIndex;
  if( (channel < MAX_SERVOS) )   // ensure channel is valid
  {
    if (value < SERVO_MIN())          // ensure pulse width is valid
      value = SERVO_MIN();
    else if (value > SERVO_MAX())
      value = SERVO_MAX();

    servos[channel].us = value;
  }
}

int Servo::read() // return the value as degrees
{
  return map(readMicroseconds()+1, SERVO_MIN(), SERVO_MAX(), 0, 180);
}

int Servo::readMicroseconds()
{
  unsigned int pulsewidth;
  if (this->servoIndex != INVALID_SERVO)
    pulsewidth = servos[this->servoIndex].us;
  else
    pulsewidth  = 0;

  return pulsewidth;
}

bool Servo::attached()
{
  return servos[this->servoIndex].Pin.isActive;
}


