/*
The quadcopter_main.info is placed under the MIT license

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
#include "system_control.h"
#include "mpuXXXX.h"
#include "radio_control.h"
#include "fly_controler.h"
#include "pid.h"
#include <wiring_os.h>

void quadcopterMain();
bool quadcopteIinit();

static unsigned int  quadcopterTid;
static bool initResult=false;




void setup() {
	initResult=quadcopterInit();
}

void loop() {
	radioTask(NULL);
}


void startQuadcopterTask(){
	  quadcopterTid = os_thread_create(quadcopterMain, NULL, OS_PRIORITY_HIGH, 2048);
	  printf("%s: quadcopterTid=%d\n",__func__,quadcopterTid);
}

void stopQuadcopterTask(){
	os_thread_terminate(quadcopterTid);
}


void quadcopterMain(const void *arg){
			long tv;
			static unsigned long  last_tv = 0;
			short count = 0;
			float yrpAttitude[3];
			float pryRate[3];
			float xyzAcc[3];
			float xyzGravity[3];
			float xyzMagnet[3];
			
	while (true) {
		while (true==initResult) {
			
		if (0 == getYawPitchRollInfo(yrpAttitude, pryRate, xyzAcc, xyzGravity,xyzMagnet)) {

			count++;
			
			setYaw(yrpAttitude[0]);
			setPitch(yrpAttitude[2]);
			setRoll(yrpAttitude[1]);
 			
			setYawGyro(-pryRate[2]);
			setPitchGyro(pryRate[0]);
			setRollGyro(-pryRate[1]);

#if 0//debug

		tv=millis();
		 printf("T duration=%d ms\n",(tv-last_tv));
		last_tv=tv;
			//	 printf("Roll_Gyro=%f, Pitch_Gyro=%f, Yaw_Gyro=%f\n", -pryRate[1], -pryRate[0], -pryRate[2]);
		// printf("Roll_Attitude=%f, Pitch_Attitude=%f, Yaw_Attitude=%f\n", getRoll(), getPitch(), getYaw());
#endif
					
			if (count >= getAdjustPeriod()) {
				if(true==flySystemIsEnable()){			
					if(getPacketAccCounter()!=MAX_COUNTER){
						if  (getPidSp(&yawAttitudePidSettings) != 321.0) {
							adjustMotor();
						}else{
							setupAllMotorPoewrLevel(getMinPowerLevel(),
								getMinPowerLevel(), getMinPowerLevel(),
								getMinPowerLevel());
							setThrottlePowerLevel(getMinPowerLevel());
						}
					}else{
						int throttleValue=max(getMinPowerLevel(),getThrottlePowerLevel()-5);
						setPidSp(&rollAttitudePidSettings,0.f);
						setPidSp(&pitchAttitudePidSettings,0.f);
						
						setupAllMotorPoewrLevel(throttleValue,
							throttleValue, throttleValue,
							throttleValue);
						setThrottlePowerLevel(throttleValue);
					}

				}else{
					setupAllMotorPoewrLevel(0, 0, 0,0);
					setThrottlePowerLevel(MIN_POWER_LEVEL);
				}
				count = 0;
			}

		}
		os_thread_yield();
	}
	}

}

bool quadcopterInit(){

  bool result=true;
  	
   if(systemInit())
   		printf( "System initialation is done!\n");
   else
		return false;
   delay(1);

  if(mpu6050Init())
   		printf( "mpuXXXX initialation is done!\n");
  else
  		return false;
  delay(1);
  
  if(motorInit())
   		printf( "motor initialation is done!\n");
  else
	   return false;
  delay(1);

  if(flyControlerInit())
   		printf( "fly comntroler initialation is done!\n");
  else
	   return false;
  delay(1);

  if(packetAccCounterInit())
   		printf( "safety machenism initialation is done!\n");
  else
	   return false;
  delay(1);

  if(pidInit())
  		printf( "PID initialation is done\n!");
  else
	   return false;
  delay(1);

  if(radioControlInit())
   		printf( "Radio initialation is done!\n");
  else
	   return false;
  delay(1);

  printf( "All initialation is done!!!!!\n");

  startRadioTransceiverTask();
  startQuadcopterTask();

  return result;
}

