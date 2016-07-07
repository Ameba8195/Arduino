/*
The radio_control.cpp is placed under the MIT license

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

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <string.h>
#include "common_lib.h"
#include "fly_controler.h"
#include "pid.h"
#include "motor_control.h"
#include "system_control.h"
#include "mpuxxxx.h"
#include "safe_machenism.h"
#include "radio_control.h"


#define TRANSMIT_TIMER 40
#define RECEIVER_BUFFER_SIZE 150
#define TRANSCEIVER_BUFFER_SIZE 150


static WiFiUDP Udp;
static unsigned int localPort = 2390;      
static char ssid[] = "Quadcopter";  
static char pass[] = "1234567890";     
static char channel[] = "4";         
static int status = WL_IDLE_STATUS;     
static unsigned int transceiverTid;
static char receiverBuffer[RECEIVER_BUFFER_SIZE*5];
static char transceiverBuffer[TRANSCEIVER_BUFFER_SIZE];
unsigned long  radio_last_tv = 0;
static IPAddress broadcastIP(255, 255, 255, 255); 


static unsigned int  radioTransmitTaskTid;


short processRadioMessages(char *buf, short lenth);
void printWifiData();
void printCurrentNet();


bool radioControlInit() {
  bool result = true;

  if (WiFi.status() == WL_NO_SHIELD) {
    printf( "WiFi shield not present\n");
    result = false;
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    printf( "Please upgrade the firmware\n");
  }

  while (status != WL_CONNECTED) {
     printf( "Attempting to start AP with SSID: %s\n",ssid);
	status = WiFi.apbegin(ssid, channel);
    delay(10000);
  }

  printf( "AP mode already started\n");
   printWifiData();
   printCurrentNet();

   printf( "Establish WiFi Server\n");
  
   Udp.begin(localPort);

  
  memset(receiverBuffer, '\0', sizeof(receiverBuffer));
  memset(transceiverBuffer, '\0', sizeof(transceiverBuffer));

  radio_last_tv=millis();

  return result;
}



static unsigned int  radioTransceiverTaskTid;
void radioTransceiverTask(const void *arg);


void startRadioTransceiverTask(){
	  radioTransceiverTaskTid = os_thread_create(radioTransceiverTask, NULL, OS_PRIORITY_HIGH, 2048);
	   printf("%s: transceiverTid=%d\n",__func__,radioTransceiverTaskTid);
}

void stopRadioTransceiverTask(){
	os_thread_terminate(radioTransceiverTaskTid);
}



void radioTransceiverTask(const void *arg) {

	unsigned long tv;
	static unsigned long  radio_last_tv2 = 0;
	while (true){

#if 1
#if 0 //debug
		tv=millis();
		 Serial.println(tv-radio_last_tv2);
		 radio_last_tv2=tv;
#endif

			memset(transceiverBuffer, '\0', sizeof(transceiverBuffer));
			sprintf(transceiverBuffer,
						"@%3.2f:%3.2f:%3.2f:%3.2f:%3.2f:%3.2f:%3.2f:%3.2f:%3.2f:%d:%d:%d:%d:%d#",
						getRoll(), getPitch(), getYaw(), getPidSp(&rollAttitudePidSettings),
						getPidSp(&pitchAttitudePidSettings),  getYawCenterPoint() + getPidSp(&yawAttitudePidSettings),
						getRollGyro(), getPitchGyro(), getYawGyro(),
						getThrottlePowerLevel(), getMotorPowerLevelCCW1(),
						getMotorPowerLevelCW1(), getMotorPowerLevelCCW2(),
						getMotorPowerLevelCW2());
	
				Udp.beginPacket(broadcastIP, localPort);
				Udp.write(transceiverBuffer,strlen(transceiverBuffer));
				Udp.endPacket();
				
			increasePacketAccCounter();
	    os_thread_yield();
		delay(TRANSMIT_TIMER);
#endif
		}

}


void radioTask(const void *arg) {

//while(true){

unsigned long tv;

  char getChar;
  char count = 0;

  char receiverBuffer2[RECEIVER_BUFFER_SIZE];

    while (true){

    if (Udp.read(receiverBuffer, sizeof(receiverBuffer)) > 0) {
	    
		for(int i=0;i<sizeof(receiverBuffer);i++){
			getChar=receiverBuffer[i];
			if (getChar == '@') {
						memset(receiverBuffer2, '\0', sizeof(receiverBuffer2));
						receiverBuffer2[0] = getChar;
						count = 1;
					} else if ((getChar == '#') && (receiverBuffer2[0] == '@')
							&& (count < sizeof(receiverBuffer2))) {
						receiverBuffer2[count] = getChar;
						processRadioMessages( receiverBuffer2, strlen(receiverBuffer2));
						memset(receiverBuffer2, '\0', sizeof(receiverBuffer2));
						count = 0;
					} else {
						if (receiverBuffer2[0] == '@' && (count < sizeof(receiverBuffer2))) {
							receiverBuffer2[count] = getChar;
							count++;
						} else {
							memset(receiverBuffer2, '\0', sizeof(receiverBuffer2));
							count = 0;
						}
					}
		}

      memset(receiverBuffer,'\0',sizeof(receiverBuffer));
	   memset(receiverBuffer2,'\0',sizeof(receiverBuffer2));
    }
	os_thread_yield();
	delay(1);
  }

}


void qToken(char *in, char *out,int bufLenth,int index){
	memset(out, '\0',bufLenth);
	int count=0;
	int j=0;
	for(int i=0;i<bufLenth  ;i++){

		if(count==index){
			if(in[i]==':'||in[i]=='\0'){
				break;
			}
			else{
			out[j]=in[i];
				j++;
			}
		}

		if(in[i]==':'){
			count++;
		}
	}
}


/**
   Decode  received packetes

   @param buf
  				received packet

   @param lenth
  				packet size
*/

short processRadioMessages(char *buf, short lenth) {
  short cmdIndex = -1;
  char cmd[lenth];
  char token2[lenth];
  short i = 0;
  short changeLeve = 0;
  short parameter = 0;
  float parameterF = 0.0;
  float rollSpShift = 0;
  float pitchSpShift = 0;
  float yawShiftValue = 0;
  float throttlePercentage = 0;
  static unsigned long  radio_last_tv3 = 0;

#if 0
   unsigned long tv=millis();
  Serial.println(tv-radio_last_tv3);
   radio_last_tv3=tv;
  // Serial.println(buf);
#endif

  resetPacketAccCounter();
  

  memset(cmd, '\0', sizeof(cmd));

  for (i = 1; buf[i] != '#'; i++) {
    cmd[i - 1] = buf[i];
  }
 
  qToken(cmd, token2,lenth, 0);
  cmdIndex = atoi(token2);


  switch (cmdIndex) {
    case ON_OFF_FLY_SYSTEM_MESSAGE:

	  qToken(cmd, token2,lenth,ON_OFF_FLY);
      parameter = atoi(token2);

      if (1 == parameter) {
          enableFlySystem();
      } else {
        disenableFlySystem();
      }

      break;
    case PILOT_CONTROL_MESSAGE:
     // printf("%s\n",buf);

	  qToken(cmd, token2,lenth, THROTTLE_LEVEL);
      parameter = atoi(token2);
	  
      qToken(cmd, token2,lenth, ROLL_SP_SHIFT);
      rollSpShift = atof(token2);

      qToken(cmd, token2,lenth, PITCH_SP_SHIFT);
      pitchSpShift = atof(token2);

       qToken(cmd, token2,lenth, YAW_SP_SHIFT);
      yawShiftValue = atof(token2);
      //printf("factor=%d\n",(int)(((float)(parameter-100)/(float)100)*(float)(MAX_POWER_LEVEL-MIN_POWER_LEVEL)));
      //printf("parameter=%d\n",parameter);
      throttlePercentage = (float)parameter / 100.f;
      parameter = getMinPowerLevel()
                  + (int) (throttlePercentage
                           * (float) (getMaxPowerLeve()
                                      - getMinPowerLevel())); /*(100~200)*10=1000~2000 us*/
      //printf("getMaxPowerLeveRange()- getMinPowerLeveRange()=%f\n",getMaxPowerLeve()- getMinPowerLevel());
      //printf("parameter=%d\n",parameter);

      if (parameter > getMaxPowerLeve()
          || parameter < getMinPowerLevel()) {
         printf( "break\n");
        break;
      }
      if (true == flySystemIsEnable()) {

        setThrottlePowerLevel(parameter);

        if (getMinPowerLevel() == parameter) {
          //printf("STOP\n");
          resetPidRecord(&rollAttitudePidSettings);
          resetPidRecord(&pitchAttitudePidSettings);
          resetPidRecord(&yawAttitudePidSettings);
          resetPidRecord(&rollRatePidSettings);
          resetPidRecord(&pitchRatePidSettings);
          resetPidRecord(&yawRatePidSettings);
          setYawCenterPoint(0);
          setPidSp(&yawAttitudePidSettings, 321.0);

        } else {
          if (getPidSp(&yawAttitudePidSettings) == 321.0) {
             printf( "START Flying\n");
            setYawCenterPoint(getYaw());
            setPidSp(&yawAttitudePidSettings, 0);
          }
          setPidSp(&rollAttitudePidSettings,
                   LIMIT_MIN_MAX_VALUE(rollSpShift, -getAngularLimit(),
                                       getAngularLimit()));
          setPidSp(&pitchAttitudePidSettings,
                   LIMIT_MIN_MAX_VALUE(pitchSpShift, -getAngularLimit(),
                                       getAngularLimit()));
          setYawCenterPoint(getYawCenterPoint() + (yawShiftValue * 1.0));

        }

      }

      //printf("throttle=%d roll=%f pitch=%f\n",parameter,rollSpShift,pitchSpShift);

      break;



    case SYSTEM_SETTING_MESSAGE:
       printf( "%s " , buf);

	  qToken(cmd, token2,lenth, ANAGULAR_VELOVITY_LIMIT);
      parameterF = atof(token2);
      if (parameterF == 0) {
        parameterF = 1;
      }
      setGyroLimit(parameterF);
	  printf( "Angular Velocity Limit: %4.3f\n",getGyroLimit());
      /***/

	  qToken(cmd, token2,lenth, ROLL_CALIBRATION);
      parameterF = atof(token2);
      setPidSpShift(&rollAttitudePidSettings, parameterF);
	  printf( "Roll Angular Calibration: %4.3f\n",getPidSpShift(&rollAttitudePidSettings));

      /***/

	  qToken(cmd, token2,lenth, PITCH_CALIBRATION);
      parameterF = atof(token2);
      setPidSpShift(&pitchAttitudePidSettings, parameterF);
	  printf( "Pitch Angular Calibration: %4.3f\n",getPidSpShift(&pitchAttitudePidSettings));


      break;

    case PID_SETTING_MESSAGE:
	
       printf("%s\n", buf);

    
	  qToken(cmd, token2,lenth, ATTITUDE_ROLL_P);
      parameterF = atof(token2);
      setPGain(&rollAttitudePidSettings, parameterF);
       printf( "Attitude Roll P Gain=%f\n", getPGain(&rollAttitudePidSettings));

      
      qToken(cmd, token2,lenth, ATTITUDE_ROLL_I);
      parameterF = atof(token2);
      setIGain(&rollAttitudePidSettings, parameterF);
      printf("Attitude Roll I Gain=%f\n", getIGain(&rollAttitudePidSettings));

    
       qToken(cmd, token2,lenth,ATTITUDE_ROLL_I_LIMIT );
      parameterF = atof(token2);
      setILimit(&rollAttitudePidSettings, parameterF);
       printf( "Attitude Roll I Output Limit=%f\n",
                getILimit(&rollAttitudePidSettings));

       qToken(cmd, token2,lenth,ATTITUDE_ROLL_D );
      parameterF = atof(token2);
      setDGain(&rollAttitudePidSettings, parameterF);
       printf("Attitude Roll D Gain=%f\n", getDGain(&rollAttitudePidSettings));

       qToken(cmd, token2,lenth,ATTITUDE_PITCH_P );
      parameterF = atof(token2);
      setPGain(&pitchAttitudePidSettings, parameterF);
       printf( "Attitude Pitch P Gain=%f\n", getPGain(&pitchAttitudePidSettings));

       qToken(cmd, token2,lenth, ATTITUDE_PITCH_I);
      parameterF = atof(token2);
      setIGain(&pitchAttitudePidSettings, parameterF);
      printf( "Attitude Pitch I Gain=%f\n", getIGain(&pitchAttitudePidSettings));

       qToken(cmd, token2,lenth, ATTITUDE_PITCH_I_LIMIT);
      parameterF = atof(token2);
      setILimit(&pitchAttitudePidSettings, parameterF);
      printf( "Attitude Pitch I Output Limit=%f\n",
                getILimit(&pitchAttitudePidSettings));

       qToken(cmd, token2,lenth, ATTITUDE_PITCH_D);
      parameterF = atof(token2);
      setDGain(&pitchAttitudePidSettings, parameterF);
       printf( "Attitude Pitch D Gain=%f\n", getDGain(&pitchAttitudePidSettings));


       qToken(cmd, token2,lenth,ATTITUDE_YAW_P );
      parameterF = atof(token2);
      setPGain(&yawAttitudePidSettings, parameterF);
       printf( "Attitude Yaw P Gain=%f\n", getPGain(&yawAttitudePidSettings));

       qToken(cmd, token2,lenth, ATTITUDE_YAW_I);
      parameterF = atof(token2);
      setIGain(&yawAttitudePidSettings, parameterF);
       printf( "Attitude Yaw I Gain=%f\n", getIGain(&yawAttitudePidSettings));

       qToken(cmd, token2,lenth, ATTITUDE_YAW_I_LIMIT);
      parameterF = atof(token2);
      setILimit(&yawAttitudePidSettings, parameterF);
       printf( "Attitude Yaw I Output Limit=%f\n", getILimit(&yawAttitudePidSettings));
	   
       qToken(cmd, token2,lenth, ATTITUDE_YAW_D);
      parameterF = atof(token2);
      setDGain(&yawAttitudePidSettings, parameterF);
       printf( "Attitude Yaw D Gain=%f\n", getDGain(&yawAttitudePidSettings));

       qToken(cmd, token2,lenth, RATE_ROLL_P);
      parameterF = atof(token2);
      setPGain(&rollRatePidSettings, parameterF);
       printf( "Rate Roll P Gain=%f\n", getPGain(&rollRatePidSettings));
    
       qToken(cmd, token2,lenth, RATE_ROLL_I);
      parameterF = atof(token2);
      setIGain(&rollRatePidSettings, parameterF);
       printf( "Rate Roll I Gain=%f\n", getIGain(&rollRatePidSettings));
     
       qToken(cmd, token2,lenth, RATE_ROLL_I_LIMIT);
      parameterF = atof(token2);
      setILimit(&rollRatePidSettings, parameterF);
       printf( "Rate Roll I Output Limit=%f\n",
                getILimit(&rollRatePidSettings));
      
       qToken(cmd, token2,lenth,RATE_ROLL_D);
      parameterF = atof(token2);
      setDGain(&rollRatePidSettings, parameterF);
      printf( "Rate Roll D Gain=%f\n", getDGain(&rollRatePidSettings));                                                                                                                                                                                                                                                     

    
       qToken(cmd, token2,lenth,RATE_PITCH_P );
      parameterF = atof(token2);
      setPGain(&pitchRatePidSettings, parameterF);
      printf( "Rate Pitch P Gain=%f\n", getPGain(&pitchRatePidSettings));
   
       qToken(cmd, token2,lenth,RATE_PITCH_I );
      parameterF = atof(token2);
      setIGain(&pitchRatePidSettings, parameterF);
       printf( "Rate Pitch I Gain=%f\n", getIGain(&pitchRatePidSettings));
     
       qToken(cmd, token2,lenth,RATE_PITCH_I_LIMIT );
      parameterF = atof(token2);
      setILimit(&pitchRatePidSettings, parameterF);
       printf( "Rate Pitch I Output Limit=%f\n",
                getILimit(&pitchRatePidSettings));
  
       qToken(cmd, token2,lenth, RATE_PITCH_D);
      parameterF = atof(token2);
      setDGain(&pitchRatePidSettings, parameterF);
       printf( "Rate Pitch D Gain=%f\n", getDGain(&pitchRatePidSettings));

    
     qToken(cmd, token2,lenth,RATE_YAW_P );
      parameterF = atof(token2);
      setPGain(&yawRatePidSettings, parameterF);
       printf( "Rate Yaw P Gain=%f\n", getPGain(&yawRatePidSettings));
     
       qToken(cmd, token2,lenth, RATE_YAW_I);
      parameterF = atof(token2);
      setIGain(&yawRatePidSettings, parameterF);
      printf( "Rate Yaw I Gain=%f\n", getIGain(&yawRatePidSettings));
    
       qToken(cmd, token2,lenth,RATE_YAW_I_LIMIT );
      parameterF = atof(token2);
      setILimit(&yawRatePidSettings, parameterF);
       printf("Rate Yaw I Output Limit=%f\n",
                getILimit(&yawRatePidSettings));

       qToken(cmd, token2,lenth, RATE_YAW_D);
      parameterF = atof(token2);
      setDGain(&yawRatePidSettings, parameterF);
       printf( "Rate Yaw D Gain=%f\n", getDGain(&yawRatePidSettings));
      break;
	  default:
	  	printf("unknow message\n");
  }

}


void printWifiData() {
 
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

 
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
  Serial.println();
}

void printCurrentNet() {
 
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());


  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[0], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.println(bssid[5], HEX);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}



