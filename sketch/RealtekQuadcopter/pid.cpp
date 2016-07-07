/*
The pid.cpp is placed under the MIT license

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

#include "pid.h"


#define DEFAULT_ROLL_ATTITUDE_P_GAIN 0.0
#define DEFAULT_ROLL_ATTITUDE_I_GAIN 0.0
#define DEFAULT_ROLL_ATTITUDE_D_GAIN 0.0
#define DEFAULT_ROLL_ATTITUDE_I_LIMIT 0.0

#define DEFAULT_PITCH_ATTITUDE_P_GAIN 0.0
#define DEFAULT_PITCH_ATTITUDE_I_GAIN 0.0
#define DEFAULT_PITCH_ATTITUDE_D_GAIN 0.0
#define DEFAULT_PITCH_ATTITUDE_I_LIMIT 0.0

#define DEFAULT_YAW_ATTITUDE_P_GAIN 0.0
#define DEFAULT_YAW_ATTITUDE_I_GAIN 0.0
#define DEFAULT_YAW_ATTITUDE_D_GAIN 0.0
#define DEFAULT_YAW_ATTITUDE_I_LIMIT 0.0

#define DEFAULT_ROLL_ATTITUDE_SP 0.0
#define DEFAULT_PITCH_ATTITUDE_SP 0.0
#define DEFAULT_YAW_ATTITUDE_SP 0.0
#define DEFAULT_ROLL_ATTITUDE_SHIFT 0.0
#define DEFAULT_PITCH_ATTITUDE_SHIFT  0.0
#define DEFAULT_YAW_ATTITUDE_SHIFT 0.0

PID_STRUCT rollAttitudePidSettings;
PID_STRUCT pitchAttitudePidSettings;
PID_STRUCT yawAttitudePidSettings;


#define DEFAULT_ROLL_RATE_P_GAIN 0.0
#define DEFAULT_ROLL_RATE_I_GAIN 0.0
#define DEFAULT_ROLL_RATE_D_GAIN 0.0
#define DEFAULT_ROLL_RATE_I_LIMIT 0.0

#define DEFAULT_PITCH_RATE_P_GAIN 0.0
#define DEFAULT_PITCH_RATE_I_GAIN 0.0
#define DEFAULT_PITCH_RATE_D_GAIN 0.0
#define DEFAULT_PITCH_RATE_I_LIMIT 0.0

#define DEFAULT_YAW_RATE_P_GAIN 0.0
#define DEFAULT_YAW_RATE_I_GAIN 0.0
#define DEFAULT_YAW_RATE_D_GAIN 0.0
#define DEFAULT_YAW_RATE_I_LIMIT 0.0

#define DEFAULT_ROLL_RATE_SP 0.0
#define DEFAULT_PITCH_RATE_SP 0.0
#define DEFAULT_YAW_RATE_SP 0.0
#define DEFAULT_ROLL_RATE_SHIFT 0.0
#define DEFAULT_PITCH_RATE_SHIFT 0.0
#define DEFAULT_YAW_RATE_SHIFT 0.0

PID_STRUCT rollRatePidSettings;
PID_STRUCT pitchRatePidSettings;
PID_STRUCT yawRatePidSettings;

#define PID_SAMPLE_TIME 0.005  



bool pidInit() {


	pidTune(&rollAttitudePidSettings, DEFAULT_ROLL_ATTITUDE_P_GAIN,
	DEFAULT_ROLL_ATTITUDE_I_GAIN, DEFAULT_ROLL_ATTITUDE_D_GAIN,
	DEFAULT_ROLL_ATTITUDE_SP, DEFAULT_ROLL_ATTITUDE_SHIFT,
	DEFAULT_ROLL_ATTITUDE_I_LIMIT, PID_SAMPLE_TIME);
	pidTune(&pitchAttitudePidSettings, DEFAULT_PITCH_ATTITUDE_P_GAIN,
	DEFAULT_PITCH_ATTITUDE_I_GAIN, DEFAULT_PITCH_ATTITUDE_D_GAIN,
	DEFAULT_PITCH_ATTITUDE_SP, DEFAULT_PITCH_ATTITUDE_SHIFT,
	DEFAULT_PITCH_ATTITUDE_I_LIMIT, PID_SAMPLE_TIME);
	pidTune(&yawAttitudePidSettings, DEFAULT_YAW_ATTITUDE_P_GAIN,
	DEFAULT_YAW_ATTITUDE_I_GAIN, DEFAULT_YAW_ATTITUDE_D_GAIN,
	DEFAULT_YAW_ATTITUDE_SP, DEFAULT_YAW_ATTITUDE_SHIFT,
	DEFAULT_YAW_ATTITUDE_I_LIMIT, PID_SAMPLE_TIME);
	setName(&rollAttitudePidSettings, "ROLL_A");
	setName(&pitchAttitudePidSettings, "PITCH_A");
	setName(&yawAttitudePidSettings, "YAW_A");
	resetPidRecord(&rollAttitudePidSettings);
	resetPidRecord(&pitchAttitudePidSettings);
	resetPidRecord(&yawAttitudePidSettings);
	setPidSp(&yawAttitudePidSettings, 321.0); //321 is just a value for yaw while while throttle is minimum


	pidTune(&rollRatePidSettings, DEFAULT_ROLL_RATE_P_GAIN,
	DEFAULT_ROLL_RATE_I_GAIN, DEFAULT_ROLL_RATE_D_GAIN,
	DEFAULT_ROLL_RATE_SP, DEFAULT_ROLL_RATE_SHIFT,
	DEFAULT_ROLL_RATE_I_LIMIT, PID_SAMPLE_TIME);
	pidTune(&pitchRatePidSettings, DEFAULT_PITCH_RATE_P_GAIN,
	DEFAULT_PITCH_RATE_I_GAIN, DEFAULT_PITCH_RATE_D_GAIN,
	DEFAULT_PITCH_RATE_SP, DEFAULT_PITCH_RATE_SHIFT,
	DEFAULT_PITCH_RATE_I_LIMIT, PID_SAMPLE_TIME);
	pidTune(&yawRatePidSettings, DEFAULT_YAW_RATE_P_GAIN,
	DEFAULT_YAW_RATE_I_GAIN, DEFAULT_YAW_RATE_D_GAIN,
	DEFAULT_YAW_RATE_SP, DEFAULT_YAW_RATE_SHIFT,
	DEFAULT_YAW_RATE_I_LIMIT, PID_SAMPLE_TIME);
	setName(&rollRatePidSettings, "ROLL_R");
	setName(&pitchRatePidSettings, "PITCH_R");
	setName(&yawRatePidSettings, "YAW_R");
	resetPidRecord(&rollRatePidSettings);
	resetPidRecord(&pitchRatePidSettings);
	resetPidRecord(&yawRatePidSettings);
return true;
}


 
float pidCalculation(PID_STRUCT *pid, float processValue) {

	float pterm = 0, dterm = 0, iterm = 0, result = 0, timeDiff=0;
	unsigned long tv;

	tv=millis();

	if (pid->last_tv != 0) {

		pid->pv = processValue;
		timeDiff=(float)(tv-pid->last_tv)/1000.f;// Sec

		//P term
		pid->err = (pid->sp + pid->spShift) - (pid->pv);

		//I term
		pid->integral += (pid->err * timeDiff);
		if (pid->integral > pid->iLimit) {
			pid->integral = pid->iLimit;
		} else if (pid->integral < -pid->iLimit) {
			pid->integral = -pid->iLimit;
		}

		//D term
		dterm = (pid->err - pid->last_error) / timeDiff;

		pterm = pid->pgain * pid->err;
		iterm = pid->igain * pid->integral;
		dterm = pid->dgain * dterm;

		result = (pterm + iterm + dterm);

	}
	
	pid->last_error = pid->err;
	pid->last_tv = tv;

	return result;
}


void pidTune(PID_STRUCT *pid, float p_gain, float i_gain, float d_gain,
		float set_point, float shift, float iLimit, float dt) {
	pid->pgain = p_gain;
	pid->igain = i_gain;
	pid->dgain = d_gain;
	pid->sp = set_point;
	pid->spShift = shift;
	pid->iLimit = iLimit;
	pid->dt = dt;
}


void resetPidRecord(PID_STRUCT *pid) {
	pid->integral = 0.f;
	pid->err = 0.f;
	pid->last_error = 0.f;
	pid->last_tv = 0;
}

void setPidError(PID_STRUCT *pi, float value) {
	pi->err = value;
}

float getPidSperror(PID_STRUCT *pi) {
	return pi->err;
}


void setPidSpShift(PID_STRUCT *pi, float value) {
	pi->spShift = value;
}


float getPidSpShift(PID_STRUCT *pi) {
	return pi->spShift;
}


void setPidSp(PID_STRUCT *pid, float set_point) {
	pid->sp = set_point;
}


float getPidSp(PID_STRUCT *pid) {
	return pid->sp;
}

void setName(PID_STRUCT *pid, char *name) {
	strcpy(pid->name, name);
}


char *getName(PID_STRUCT *pid) {
	return pid->name;
}


void setPGain(PID_STRUCT *pid, float gain) {
	pid->pgain = gain;
}


float getPGain(PID_STRUCT *pid) {
	return pid->pgain;
}


void setIGain(PID_STRUCT *pid, float gain) {
	pid->igain = gain;
}


float getIGain(PID_STRUCT *pid) {
	return pid->igain;
}


void setILimit(PID_STRUCT *pid, float v) {
	pid->iLimit = v;
}


float getILimit(PID_STRUCT *pid) {
	return pid->iLimit;
}

void setDGain(PID_STRUCT *pid, float gain) {
	pid->dgain = gain;
}


float getDGain(PID_STRUCT *pid) {
	return pid->dgain;
}


