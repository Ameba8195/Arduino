/*
The radio_control.h is placed under the MIT license

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

typedef enum {
	NONE_0,
	ON_OFF_FLY_SYSTEM_MESSAGE,
	PILOT_CONTROL_MESSAGE,
	NONE_3,
	SYSTEM_SETTING_MESSAGE,
	NONE_5,
	PID_SETTING_MESSAGE
}DOWNLINK_MESSAGE_TYPE;

typedef enum {
	ON_OFF_FLY_SYSTEM_HEADER,
	ON_OFF_FLY
}ON_OFF_FLY_SYSTEM;


typedef enum {
	PILOT_CONTROL_HEADER,
	THROTTLE_LEVEL,
	ROLL_SP_SHIFT,
	PITCH_SP_SHIFT,
	YAW_SP_SHIFT
}PILOT_CONTROL;


typedef enum {
	SYSTEM_SETTING_HEADER,
	ANAGULAR_VELOVITY_LIMIT,
	ROLL_CALIBRATION,
	PITCH_CALIBRATION
}SYSTEM_SETTING;

typedef enum {
	PID_SETTING_HEADER,
	ATTITUDE_ROLL_P,
	ATTITUDE_ROLL_I,
	ATTITUDE_ROLL_I_LIMIT,
	ATTITUDE_ROLL_D,
	ATTITUDE_PITCH_P,
	ATTITUDE_PITCH_I,
	ATTITUDE_PITCH_I_LIMIT,
	ATTITUDE_PITCH_D,
	ATTITUDE_YAW_P,
	ATTITUDE_YAW_I,
	ATTITUDE_YAW_I_LIMIT,
	ATTITUDE_YAW_D,
	RATE_ROLL_P,
	RATE_ROLL_I,
	RATE_ROLL_I_LIMIT,
	RATE_ROLL_D,
	RATE_PITCH_P,
	RATE_PITCH_I,
	RATE_PITCH_I_LIMIT,
	RATE_PITCH_D,
	RATE_YAW_P,
	RATE_YAW_I,
	RATE_YAW_I_LIMIT,
	RATE_YAW_D
}PID_SETTING;


bool radioControlInit();
void radioTask(const void *arg);
void startRadioTransceiverTask();
void stopRadioTransceiverTask();






