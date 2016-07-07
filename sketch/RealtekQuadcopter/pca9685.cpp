/*
The pca9685.c is placed under the MIT license

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

#include "./I2Cdev/I2Cdev.h"
#include "pca9685.h"

#define MODE1 0x00			
#define MODE2 0x01			
#define SUBADR1 0x02		
#define SUBADR2 0x03		
#define SUBADR3 0x04		
#define ALLCALLADR 0x05    
#define LED0 0x6			
#define LED0_ON_L 0x6		
#define LED0_ON_H 0x7		
#define LED0_OFF_L 0x8		
#define LED0_OFF_H 0x9		
#define LED_MULTIPLYER 4	
#define ALLLED_ON_L 0xFA    
#define ALLLED_ON_H 0xFB	
#define ALLLED_OFF_L 0xFC	
#define ALLLED_OFF_H 0xFD	
#define PRE_SCALE 0xFE		
#define CLOCK_FREQ 25000000.0 
static bool PCA9685_initSuccess;

void pca9685Init() {
	PCA9685_initSuccess = false;
	if(true== testPca9685Connection()){
		PCA9685_initSuccess = true;
		resetPca9685();
	}
}


void resetPca9685() {
	if(true==PCA9685_initSuccess){
		printf( "resetPca9685\n");
		I2Cdev::writeByte(PCA9685_ADDRESS, MODE1, 0x00);
		I2Cdev::writeByte(PCA9685_ADDRESS, MODE2, 0x04);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_L + LED_MULTIPLYER * 0, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_H + LED_MULTIPLYER * 0, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_L + LED_MULTIPLYER * 1, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_H + LED_MULTIPLYER * 1, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_L + LED_MULTIPLYER * 2, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_H + LED_MULTIPLYER * 2, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_L + LED_MULTIPLYER * 3, 0);
		I2Cdev::writeByte(PCA9685_ADDRESS, LED0_ON_H + LED_MULTIPLYER * 3, 0);
		printf( "resetPca9685 writeByte done\n");


	}else{
		 printf("pca9685 doesn't init\n");
	}
}

bool testPca9685Connection() {
//TODO
return true;
}

void setPWMFreq(unsigned short freq) {

	 printf("PCA9685: setting frequency %d HZ\n",freq);
	unsigned char prescale = (CLOCK_FREQ / 4096 / freq) - 1;
	unsigned char oldmode = 0;
	I2Cdev::readByte(PCA9685_ADDRESS, MODE1, &oldmode);
	 printf("PCA9685: old oled mode: %d\n",oldmode);
	unsigned char newmode = (oldmode & 0x7F) | 0x10; 

	I2Cdev::writeByte(PCA9685_ADDRESS, MODE1, newmode); 
	I2Cdev::writeByte(PCA9685_ADDRESS, PRE_SCALE, prescale);
	I2Cdev::writeByte(PCA9685_ADDRESS, MODE1, oldmode); 
	delay(1);	
	I2Cdev::writeByte(PCA9685_ADDRESS, MODE1, oldmode | 0x80); 
	delay(1);	
}

void setPWM(unsigned char channel, unsigned short value) {

	//printf("channel = %d, value = %d\n",channel,value);
	
	if (0 == PCA9685_initSuccess) {
		printf("%s: PCA9685_initSuccess=%d\n", __func__, PCA9685_initSuccess);
		return;
	}
	
	I2Cdev::writeByte(PCA9685_ADDRESS, LED0_OFF_L + LED_MULTIPLYER * channel, value & 0xFF);
	I2Cdev::writeByte(PCA9685_ADDRESS, LED0_OFF_H + LED_MULTIPLYER * channel, value >> 8);
}

