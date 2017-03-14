/*
 Software serial test

 Receives from serial RX, and then sends to serial TX
 baud rate: 38400, data bits: 7, parity: even, 1 stop bit, with RTS/CTS

 The circuit: (BOARD RTL8195A)
 * RX is digital pin 0 (connect to TX of other device)
 * TX is digital pin 1 (connect to RX of other device)
 * RTS is digital pin 6 (connect to CTS of other device)
 * CTS is digital pin 2 (connect to RTS of other device)

 The circuit: (BOARD RTL8710)
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 12 (connect to RX of other device)
 * RTS is digital pin 11 (connect to CTS of other device)
 * CTS is digital pin 13 (connect to RTS of other device)
 */
#include <SoftwareSerial.h>

#if defined(BOARD_RTL8195A)
SoftwareSerial mySerial(0, 1); // RX, TX
#elif defined(BOARD_RTL8710)
SoftwareSerial mySerial(10, 12); // RX, TX
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
#if defined(BOARD_RTL8195A)
  mySerial.begin(38400, 7, PARITY_EVEN, 1, FLOW_CONTROL_RTSCTS, 6, 2);
#elif defined(BOARD_RTL8710)
  mySerial.begin(38400, 7, PARITY_EVEN, 1, FLOW_CONTROL_RTSCTS, 11, 13);
#else
  mySerial.begin(38400, 7, PARITY_EVEN, 1, FLOW_CONTROL_RTSCTS, 6, 2);
#endif
  mySerial.println("Hello, world?");
}

void loop() { // run over and over
  if (mySerial.available()) {
    mySerial.write(mySerial.read());
  }
}
