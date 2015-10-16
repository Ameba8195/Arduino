
#include "arduino.h"
#include "Ticker.h"

#define LED1  13
#define LED2  12

Ticker flipper, flipper1;
int led2_status;
void flip()  {
  led2_status = !led2_status;
  digitalWrite(LED2, led2_status);
}

void flip1()  {
  digitalWrite(LED1, !digitalRead(LED1));
}

// the setup function runs once when you press reset or power the board
void setup() {
   //
  led2_status = HIGH;
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
   
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  flipper.attach(&flip, 1);
  flipper1.attach(&flip1, 0.1);
}

// the loop function runs over and over again forever
void loop() {
  delay(1000);
}

