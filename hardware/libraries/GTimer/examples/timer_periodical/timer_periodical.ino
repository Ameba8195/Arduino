/*
  This sketch shows how to use hardware timer and invoke interrupt handler periodically
*/

#include <GTimer.h>

int counter = 0;

void myhandler(uint32_t data) {
  counter++;
  Serial.print("counter: ");
  Serial.println(counter);
}

void setup() {
  // timerid 0, period 1s, invoke myhander
  GTimer.begin(0, 1 * 1000 * 1000, myhandler);
}

void loop() {
  delay(1000);
}