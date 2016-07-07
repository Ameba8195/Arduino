/*
The fly_controler.cpp is placed under the MIT license

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


#include "common_lib.h"
#include "safe_machenism.h"
#include "motor_control.h"
#include "pid.h"
#include "system_control.h"
#include "mpuxxxx.h"
#include "fly_controler.h"





#define DEFAULT_ADJUST_PERIOD 1
#define DEFAULT_GYRO_LIMIT 20
#define DEFAULT_ANGULAR_LIMIT 1000



static float getThrottleOffset();
static void getAttitudePidOutput();

static bool leaveFlyControler;
static float rollAttitudeOutput;
static float pitchAttitudeOutput;
static float yawAttitudeOutput;
static float verticalHeightOutput;
static unsigned short adjustPeriod;
static float angularLimit;
static float gyroLimit;
static float yawCenterPoint;
static float cosz;


bool flyControlerInit(){
	setAdjustPeriod(DEFAULT_ADJUST_PERIOD);
	setGyroLimit(DEFAULT_GYRO_LIMIT);
	setAngularLimit(DEFAULT_ANGULAR_LIMIT);
	setZAxisDegree(0.0);
	rollAttitudeOutput = 0;
	pitchAttitudeOutput = 0;
	yawAttitudeOutput = 0;
	verticalHeightOutput=0;
	return true;
}



void getPidOutput(float *rollRateOutput, float *pitchRateOutput,float *yawRateOutput){

	rollAttitudeOutput = pidCalculation(&rollAttitudePidSettings, getRoll());
	rollAttitudeOutput = LIMIT_MIN_MAX_VALUE(rollAttitudeOutput,
			-getGyroLimit(), getGyroLimit());
	pitchAttitudeOutput = pidCalculation(&pitchAttitudePidSettings, getPitch());
	pitchAttitudeOutput = LIMIT_MIN_MAX_VALUE(pitchAttitudeOutput,
			-getGyroLimit(), getGyroLimit());
	yawAttitudeOutput = pidCalculation(&yawAttitudePidSettings, yawTransform(getYaw()));
	yawAttitudeOutput = LIMIT_MIN_MAX_VALUE(yawAttitudeOutput,
			-getGyroLimit(), getGyroLimit());
	setPidSp(&rollRatePidSettings, rollAttitudeOutput);
	setPidSp(&pitchRatePidSettings, pitchAttitudeOutput);
	setPidSp(&yawRatePidSettings, yawAttitudeOutput);

	*rollRateOutput = pidCalculation(&rollRatePidSettings, getRollGyro());
	*pitchRateOutput = pidCalculation(&pitchRatePidSettings, getPitchGyro());
	*yawRateOutput = pidCalculation(&yawRatePidSettings, getYawGyro());
	
}


void adjustMotor() {

	float rollRateOutput = 0.f;
	float rollCcw1 = 0.f;
	float rollCcw2 = 0.f;
	float rollCw1 = 0.f;
	float rollCw2 = 0.f;

	float pitchRateOutput = 0.f;
	float pitchCcw1 = 0.f;
	float pitchCcw2 = 0.f;
	float pitchCw1 = 0.f;
	float pitchCw2 = 0.f;

	float yawRateOutput = 0.f;
	float yawCcw1 = 0.f;
	float yawCcw2 = 0.f;
	float yawCw1 = 0.f;
	float yawCw2 = 0.f;

	float outCcw1 = 0.f;
	float outCcw2 = 0.f;
	float outCw1 = 0.f;
	float outCw2 = 0.f;

	float maxLimit = 0.f;
	float minLimit = 0.f;
	float throttleOffset = 0.f;

	//throttleOffset=getThrottleOffset();
	
	maxLimit = (float) min(
			(getThrottlePowerLevel() + throttleOffset) + getAdjustPowerLeveRange(),
			getMaxPowerLeve());
	minLimit = (float) max(
			(getThrottlePowerLevel() + throttleOffset) - getAdjustPowerLeveRange(),
			getMinPowerLevel());

	getPidOutput(&rollRateOutput, &pitchRateOutput, &yawRateOutput);

	rollCcw1 = rollRateOutput;
	rollCcw2 = -rollRateOutput;
	rollCw1 = -rollRateOutput;
	rollCw2 = rollRateOutput;

	pitchCcw1 = -pitchRateOutput;
	pitchCcw2 = pitchRateOutput;
	pitchCw1 = -pitchRateOutput;
	pitchCw2 = pitchRateOutput;

	if (getPidSp(&yawAttitudePidSettings) != 321.0) {

		yawCcw1 = yawRateOutput;
		yawCcw2 = yawRateOutput;
		yawCw1 = -yawRateOutput;
		yawCw2 = -yawRateOutput;
	}

	outCcw1 =
			((float) getThrottlePowerLevel()+ throttleOffset) + LIMIT_MIN_MAX_VALUE(rollCcw1+pitchCcw1+yawCcw1,-getAdjustPowerLimit(),getAdjustPowerLimit());
	outCcw2 =
			((float) getThrottlePowerLevel()+ throttleOffset) + LIMIT_MIN_MAX_VALUE(rollCcw2+pitchCcw2+yawCcw2,-getAdjustPowerLimit(),getAdjustPowerLimit());
	outCw1 =
			((float) getThrottlePowerLevel()+ throttleOffset) + LIMIT_MIN_MAX_VALUE(rollCw1+pitchCw1+yawCw1,-getAdjustPowerLimit(),getAdjustPowerLimit());
	outCw2 =
			((float) getThrottlePowerLevel()+ throttleOffset) + LIMIT_MIN_MAX_VALUE(rollCw2+pitchCw2+yawCw2,-getAdjustPowerLimit(),getAdjustPowerLimit());

	outCcw1 =
			 LIMIT_MIN_MAX_VALUE(outCcw1, minLimit, maxLimit);
	outCcw2 =
			 LIMIT_MIN_MAX_VALUE(outCcw2, minLimit, maxLimit);
	outCw1 =  LIMIT_MIN_MAX_VALUE(outCw1, minLimit, maxLimit);
	outCw2 =  LIMIT_MIN_MAX_VALUE(outCw2, minLimit, maxLimit);

	setupCcw1MotorPoewrLevel((short) outCcw1);
	setupCcw2MotorPoewrLevel((short) outCcw2);
	setupCw1MotorPoewrLevel((short) outCw1);
	setupCw2MotorPoewrLevel((short) outCw2);

}


float getZAxisDegree(){
	return cosz;
}

void setZAxisDegree(float degree){
	cosz=degree;
}


float getThrottleOffset() {
	float offset = 0;
	if (getZAxisDegree() <= 0.0) {
		offset = 0.0;
	} else {
		offset = (((1.0 - getZAxisDegree()) >= 0.3) ? 0.3 : 1.0 - getZAxisDegree())
				* ((float) (getThrottlePowerLevel() - getMinPowerLevel()));
	}
	return offset;
}


void setYawCenterPoint(float point){
	float yawCenterPoint1=point;
	if(yawCenterPoint1>180.0){
		yawCenterPoint1=yawCenterPoint1-360.0;
	}else if(yawCenterPoint1<-180.0){
		yawCenterPoint1=yawCenterPoint1+360.0;
	}
	yawCenterPoint=yawCenterPoint1;
}


float getYawCenterPoint(){
        return yawCenterPoint;
}

float yawTransform(float originPoint){
	float output=originPoint-yawCenterPoint;
	if(output>180.0){
		output=output-360.0;
	}else if(output<-180.0){
		output=output+360.0;
	}
	return output;
}

void setGyroLimit(float v) {
	gyroLimit = v;
}

float getGyroLimit() {
	return gyroLimit;
}

unsigned short getAdjustPeriod() {
	return adjustPeriod;
}

void setAdjustPeriod(unsigned short ms) {
	adjustPeriod = ms;
}

float getAngularLimit() {
	return angularLimit;
}

void setAngularLimit(float angular) {
	angularLimit = angular;
}




