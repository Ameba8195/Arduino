/*
The mpuXXXX.cpp is  placed under the MIT license

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
2. Some functions in this file are adapted from http://www.i2cdevlib.com/devices/mpu6050#source
*/



#if 1

#include "common_lib.h"
#include "./MPU6050/MPU6050_9Axis_MotionApps41.h"
#include "mpuxxxx.h"

MPU6050 mpu;
static float yaw;
static float pitch;
static float roll;
static float yawGyro;
static float pitchGyro;
static float rollGyro;
static float asaX;
static float asaY;
static float asaZ;

static unsigned char *dmpPacketBuffer;
static unsigned short dmpPacketSize;
volatile bool mpuInterrupt = false;     
bool blinkState = false;
bool dmpReady = false;  
uint8_t mpuIntStatus;   
uint8_t devStatus;     
uint16_t packetSize;    
uint16_t fifoCount;     
uint8_t fifoBuffer[64]; 
Quaternion q;           
VectorInt16 aa;        
VectorInt16 aaReal;    
VectorInt16 aaWorld;    
VectorFloat gravity;    
float euler[3];         
float ypr[3];           
int16_t rate[3];




void setYaw(float t_yaw) {
	yaw = t_yaw;
}

void setPitch(float t_pitch) {
	pitch = t_pitch;
}

void setRoll(float t_roll) {
	roll = t_roll;
}

float getYaw() {
	return yaw;
}

float getPitch() {
	return pitch;
}

float getRoll() {
	return roll;
}

void setYawGyro(float t_yaw_gyro) {
	yawGyro = t_yaw_gyro;
}

void setPitchGyro(float t_pitch_gyro) {
	pitchGyro = t_pitch_gyro;
}

void setRollGyro(float t_roll_gyro) {
	rollGyro = t_roll_gyro;
}

float getYawGyro() {
	return yawGyro;
}

float getPitchGyro() {
	return pitchGyro;
}

float getRollGyro() {
	return rollGyro;
}




void dmpDataReady() {
    mpuInterrupt = true;
}


bool mpu6050Init() {
printf("Initializing I2C devices...\n");
   mpu.initialize();

   // load and configure the DMP
   printf("Initializing DMP...\n");
   devStatus = mpu.dmpInitialize();
   
   // supply your own gyro offsets here, scaled for min sensitivity
   mpu.setXGyroOffset(0);
   mpu.setYGyroOffset(0);
   mpu.setZGyroOffset(0);

   // make sure it worked (returns 0 if so)
   if (devStatus == 0) {
	   // turn on the DMP, now that it's ready
	 printf("Enabling DMP...\n");
	   mpu.setDMPEnabled(true);

	   // enable Arduino interrupt detection
	   printf("Enabling interrupt detection (Ameba D3 pin)...\n");
	   attachInterrupt(3, dmpDataReady, RISING);
	   mpuIntStatus = mpu.getIntStatus();

	   // set our DMP Ready flag so the main loop() function knows it's okay to use it
	   printf("DMP ready! Waiting for first interrupt...\n");
	   dmpReady = true;

	   // get expected DMP packet size for later comparison
	   packetSize = mpu.dmpGetFIFOPacketSize();

	   /* We should avoid send/recv I2C data while there is an interrupt invoked.
	* Otherwise the MPU6050 would hang and need a power down/up reset.
	* So we set this vale big enough that we can finish task before next interrupt happend.
	*/
	   mpu.setRate(3); // 1khz / (1 + 3) = 200 Hz

   // configure LED for output
   pinMode(LED_PIN, OUTPUT);
   return true;
	   
   } else {
	   // ERROR!
	   // 1 = initial memory load failed
	   // 2 = DMP configuration updates failed
	   // (if it's going to break, usually the code will be 1)
	   printf("DMP Initialization failed (code %d)\n",devStatus);
	   return false;
   }


	
}



unsigned char getYawPitchRollInfo(float *yprAttitude, float *yprRate, float *xyzAcc, 
		float *xyzGravity, float *xyzMagnet) {

if (!dmpReady) return -1;

   // wait for MPU interrupt or extra packet(s) available
   if (!mpuInterrupt && fifoCount < packetSize) {
	   return -1;
   }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        printf("FIFO overflow!");
		return 2;

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        while (fifoCount >= packetSize){
            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
        }


     	mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		mpu.dmpGetGyro(rate, fifoBuffer);

		yprRate[0] = (float) rate[0];
		yprRate[1] = (float) rate[1];
		yprRate[2] = (float) rate[2];
		yprAttitude[0] = ypr[0] * RAD;
		yprAttitude[1] = ypr[1] *RAD;
		yprAttitude[2] = ypr[2] * RAD;
		//printf("yprAttitude[0]=%f,yprAttitude[1]=%f,yprAttitude[2]=%f\n",yprAttitude[0],yprAttitude[3],yprAttitude[2]);

        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
		return 0;
    }



}
#endif
