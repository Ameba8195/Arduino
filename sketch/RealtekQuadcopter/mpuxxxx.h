/*
The mpuxxxx.h.c is placed under the MIT license

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

#if 1

#define MPU6050_9AXIS 
#define MPU_DMP

bool mpu6050Init();
unsigned char getYawPitchRollInfo(float *yprAttitude, float *yprRate, float *xyzAcc, float *xyzGravity,float *xyzMagnet);
void setYaw(float t_yaw);
void setPitch(float t_pitch);
void setRoll(float t_roll);
float getYaw();
float getPitch();
float getRoll();
void setYawGyro(float t_yaw_gyro);
void setPitchGyro(float t_pitch_gyro);
void setRollGyro(float t_roll_gyro);
float getYawGyro();
float getPitchGyro();
float getRollGyro();
#endif





