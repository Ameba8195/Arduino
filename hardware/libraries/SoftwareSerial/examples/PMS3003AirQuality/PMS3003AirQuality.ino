/*
 This example demonstrate how to read pm2.5 value on PMS 3003 air condition sensor

 PMS 3003 pin map is as follow:
    PIN1  :VCC, connect to 5V
    PIN2  :GND
    PIN3  :SET, 0:Standby mode, 1:operating mode
    PIN4  :RXD :Serial RX
    PIN5  :TXD :Serial TX
    PIN6  :RESET
    PIN7  :NC
    PIN8  :NC

 In this example, we only use Serial to get PM 2.5 value.

 The circuit:
 * RX is digital pin 0 (connect to TX of PMS 3003)
 * TX is digital pin 1 (connect to RX of PMS 3003)

 */

#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // RX, TX

#define pmsDataLen 32
uint8_t buf[pmsDataLen];
int idx = 0;
int pm25 = 0;

void setup() {
  Serial.begin(57600);
  mySerial.begin(9600); // PMS 3003 UART has baud rate 9600
}

void loop() { // run over and over
  idx = 0;
  memset(buf, 0, pmsDataLen);

  while (mySerial.available()) {
    buf[idx++] = mySerial.read();
  }

  // check if data header is correct
  if (buf[0] == 0x42 && buf[1] == 0x4d) {
    pm25 = ( buf[12] << 8 ) | buf[13]; 
    Serial.print("pm2.5: ");
    Serial.print(pm25);
    Serial.println(" ug/m3");
  }
}

