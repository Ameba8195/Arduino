/*
The motorControl.h is placed under the MIT license

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

typedef enum{
 SOFT_PWM_CCW1,
 SOFT_PWM_CW1,
 SOFT_PWM_CCW2,
 SOFT_PWM_CW2
}MOTOR_INDEX;



#define DEFAULT_ADJUST_POWER_RANGE 1000  
#define DEFAULT_ADJUST_POWER_LIMIT 1000  

#define PWM_PWM_FREQ  490
#define MAX_POWER_LEVEL 4014    
#define MIN_POWER_LEVEL 2014 


bool motorInit();
void setupAllMotorPoewrLevel(unsigned short CW1, unsigned short CW2,
		unsigned short CCW1, unsigned short CCW2);
unsigned short getMotorPowerLevelCW1();
unsigned short getMotorPowerLevelCW2();
unsigned short getMotorPowerLevelCCW1();
unsigned short getMotorPowerLevelCCW2();
void setupCcw1MotorPoewrLevel(unsigned short CCW1);
void setupCcw2MotorPoewrLevel(unsigned short CCW2);
void setupCw1MotorPoewrLevel(unsigned short CW1);
void setupCw2MotorPoewrLevel(unsigned short CW2);
void setThrottlePowerLevel(unsigned short level);
unsigned short getThrottlePowerLevel();
unsigned short getMinPowerLevel();
unsigned short getMaxPowerLeve();
unsigned short getAdjustPowerLeveRange();
void setAdjustPowerLeveRange(int v);
unsigned short getAdjustPowerLimit();
void setAdjustPowerLimit(int v);
float getMotorGain(short index);
void setMotorGain(short index, float value);



