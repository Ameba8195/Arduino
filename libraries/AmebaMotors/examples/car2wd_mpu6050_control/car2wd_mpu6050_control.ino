#include <WiFi.h>
#include <WiFiUdp.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

/* WiFi related variables*/
char ssid[] = "mycar"; //  your network SSID (name)
char pass[] = "12345678";    // your network password (use for WPA, or use as key for WEP)
IPAddress server(192,168,1,1);  // numeric IP for mycar AP mode
int status = WL_IDLE_STATUS;

WiFiUDP Udp;
char sendbuf[12];

/* MPU6050 related variables */
MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

/* car control variable */
int carx = 0;
int cary = 0;
int prev_carx = 0;
int prev_cary = 0;
uint32_t last_send_timestamp = 0;
uint32_t timestamp = 0;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) {
      break;
    }
    // wait 1 seconds for reconnect
    delay(1000);
  }
  Serial.println("Connected to wifi");

  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock (200kHz if CPU is 8MHz).

  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Ameba D16 pin)..."));
    attachInterrupt(16, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
    
    mpu.setRate(19); // 1khz / (1 + 99) = 10 Hz
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {
    // other program behavior stuff here
    // .
    // .
    // .
    // if you are really paranoid you can frequently test in between other
    // stuff to see if mpuInterrupt is true, and if so, "break;" from the
    // while() loop to immediately process the MPU data
    // .
    // .
    // .
    os_thread_yield(); // without yield, the empty busy loop might make CPU behave un-expected
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
    Serial.println(F("FIFO overflow!"));

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

    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    mapPitchRolltoXY(ypr[1], ypr[2], &carx, &cary);
    memset(sendbuf, 0, 12);
    timestamp = millis();
    if ((timestamp - last_send_timestamp) > 500 || (carx != prev_carx && cary != prev_cary)) {
      sprintf(sendbuf, "X:%dY:%d", carx, cary);
    } else if (carx != prev_carx) {
      sprintf(sendbuf, "X:%d", carx);
    } else if (cary != prev_cary) {
      sprintf(sendbuf, "Y:%d", cary);
    }
    if (strlen(sendbuf) > 0) {
      status = WiFi.status();
      while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        if (status == WL_CONNECTED) {

          // reconnect may take a long time, reset FIFO anyway to avoid buffer overflow
          mpu.resetFIFO();
          mpuInterrupt = false;

          Serial.println("Connected to wifi");
          break;
        }
        // wait 1 seconds for reconnect
        delay(1000);
      }

      Udp.beginPacket("192.168.1.1", 5001);
      Udp.write(sendbuf);
      Udp.endPacket();

      last_send_timestamp = timestamp;
      delay(10);
    }
    prev_carx = carx;
    prev_cary = cary;
  }
}

void mapPitchRolltoXY(float pitch, float roll, int *carx, int *cary) {
  pitch = pitch * 180 / M_PI;
  roll = roll * 180 / M_PI;
  if (pitch > 24) {
    pitch = 24;
  }
  if (pitch < -24) {
    pitch = -24;
  }
  if (roll > 24) {
    roll = 24;
  }
  if (roll < -24) {
    roll = -24;
  }

  if (pitch > 12) {
    *carx = (int)(pitch-12);
  } else if (pitch < -12) {
    *carx = (int)(pitch + 12);
  } else {
    *carx = 0;
  }
  *carx = -*carx;

  if (roll > 12) {
    *cary = (int)(roll-12);
  } else if (roll < -12) {
    *cary = (int)(roll+12);
  } else {
    *cary = 0;
  }
  *cary = -*cary;
}

void checkAndReconnectWiFi() {
  status = WiFi.status();
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) {
      break;
    }
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
}
