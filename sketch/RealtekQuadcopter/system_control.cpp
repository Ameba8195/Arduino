/*
The system_control.cpp is placed under the MIT license

Copyright (c) 2016 Wu Tung Cheng, Realtek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Notes:
1. This quadcopter project is adapted from Raspberry Pilot (Github: https://github.com/jellyice1986/RaspberryPilot).

*/

#include <Arduino.h>
#include <Wire.h>


#include "common_lib.h"
#include "fly_controler.h"
#include "system_control.h"

static bool flySystemIsEnableflag;





bool systemInit() {
  disenableFlySystem();

    pinMode(18, OUTPUT);

	Serial.begin(9600);
    Wire.begin();
	Wire.setClock(400000);

  return true;
}


void enableFlySystem() {
	printf( "enableFlySystem\n");
	flySystemIsEnableflag = true;
	digitalWrite(18, HIGH);
}


void disenableFlySystem() {
	printf( "disenableFlySystem\n");
	flySystemIsEnableflag = false;
	digitalWrite(18, LOW); 
}

bool flySystemIsEnable() {
	return flySystemIsEnableflag;
}


