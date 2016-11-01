/*
 Software serial test

 Receives from serial RX, and then sends to serial TX

 The circuit: (BOARD RTL8195A)
 * RX is digital pin 0 (connect to TX of other device)
 * TX is digital pin 1 (connect to RX of other device)

 */
#include <SoftwareSerial.h>

#if defined(BOARD_RTL8195A)
SoftwareSerial mySerial(0, 1); // RX, TX
#elif defined(BOARD_RTL8710)
SoftwareSerial mySerial(17, 5); // RX, TX
#else
SoftwareSerial mySerial(0, 1); // RX, TX
#endif

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello, world?");
}

void loop() { // run over and over
  if (mySerial.available()) {
    mySerial.write(mySerial.read());
  }
}

