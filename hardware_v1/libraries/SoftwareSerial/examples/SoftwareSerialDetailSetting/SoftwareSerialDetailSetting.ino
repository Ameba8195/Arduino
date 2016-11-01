/*
 Software serial test

 Receives from serial RX, and then sends to serial TX
 data bits: 7, parity: even, 1 stop bit, with RTS/CTS

 The circuit:
 * RX is digital pin 0 (connect to TX of other device)
 * TX is digital pin 1 (connect to RX of other device)
 * RTS is digital pin 6 (connect to CTS of other device)
 * CTS is digital pin 3 (connect to RTS of other device)
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(38400, 7, PARITY_EVEN, 1, FLOW_CONTROL_RTSCTS, 6, 3);
  mySerial.println("Hello, world?");
}

void loop() { // run over and over
  if (mySerial.available()) {
    mySerial.write(mySerial.read());
  }
}
