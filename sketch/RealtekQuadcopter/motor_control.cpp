/*
The motorControl.cpp is placed under the MIT license

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



#include "pca9685.h"
#include "motor_control.h"


static unsigned short motorPowerLevel_CW1;
static unsigned short motorPowerLevel_CW2;
static unsigned short motorPowerLevel_CCW1;
static unsigned short motorPowerLevel_CCW2;
static unsigned short ThrottlePowertlevel;
static unsigned short adjustPowerLevelRange;
static unsigned short adjustPowerLimit;
static float motor_0_gain;
static float motor_1_gain;
static float motor_2_gain;
static float motor_3_gain;


bool motorInit() {
	pca9685Init();
	setAdjustPowerLeveRange(DEFAULT_ADJUST_POWER_RANGE);
	setAdjustPowerLimit(DEFAULT_ADJUST_POWER_LIMIT);
	setPWMFreq(PWM_PWM_FREQ);
		setupAllMotorPoewrLevel(0, 0, 0,
			0);
	setThrottlePowerLevel(MIN_POWER_LEVEL);
	delay(1);
	return true;
}


void setupAllMotorPoewrLevel(unsigned short CW1, unsigned short CW2,
		unsigned short CCW1, unsigned short CCW2) {
		
	setupCcw1MotorPoewrLevel(CCW1);
	setupCcw2MotorPoewrLevel(CCW2);
	setupCw1MotorPoewrLevel(CW1);
	setupCw2MotorPoewrLevel(CW2);
}


void setupCcw1MotorPoewrLevel(unsigned short CCW1) {
		motorPowerLevel_CCW1 = CCW1;
	setPWM(SOFT_PWM_CCW1, motorPowerLevel_CCW1);
}


void setupCcw2MotorPoewrLevel(unsigned short CCW2) {
		motorPowerLevel_CCW2 = CCW2;
	setPWM(SOFT_PWM_CCW2, motorPowerLevel_CCW2);
}


void setupCw1MotorPoewrLevel(unsigned short CW1) {

		motorPowerLevel_CW1 = CW1;
	setPWM(SOFT_PWM_CW1, motorPowerLevel_CW1);
}


void setupCw2MotorPoewrLevel(unsigned short CW2) {

		motorPowerLevel_CW2 = CW2;
	setPWM(SOFT_PWM_CW2, motorPowerLevel_CW2);
}


unsigned short getMotorPowerLevelCW1() {
	return motorPowerLevel_CW1;
}

unsigned short getMotorPowerLevelCW2() {
	return motorPowerLevel_CW2;
}


unsigned short getMotorPowerLevelCCW1() {
	return motorPowerLevel_CCW1;
}


unsigned short getMotorPowerLevelCCW2() {
	return motorPowerLevel_CCW2;
}


unsigned short getThrottlePowerLevel() {
	return ThrottlePowertlevel;
}


void setThrottlePowerLevel(unsigned short level) {
	ThrottlePowertlevel = level;
}


unsigned short getMinPowerLevel() {
	return MIN_POWER_LEVEL;
}


unsigned short getMaxPowerLeve() {
	return MAX_POWER_LEVEL;
}


unsigned short getAdjustPowerLeveRange() {
	return adjustPowerLevelRange;
}


void setAdjustPowerLeveRange(int v) {
	adjustPowerLevelRange = (unsigned short) v;
}


unsigned short getAdjustPowerLimit() {
	return adjustPowerLimit;
}

void setAdjustPowerLimit(int v) {
	adjustPowerLimit = (unsigned short) v;
}

