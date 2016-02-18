/*
 * Demonstrates use of sleep api of power management
 *
 * This sketch enable sleep mode. Ameba will automatically suspend when there
 * is no on-going task. For example Ameba suspend when delay() is invoked and 
 * no interrupt happened.
 *
 * NOTE: Ameba won't suspend in a busy delay. So it'd better to put a delay() in loop().
 *
 * If the sketch need use any peripheral (Ex. UART, I2C, SPI, ...), then it needs reserve
 * PLL. But it makes Ameba save less power (around 5.5 mA).
 *
 */

#include <PowerManagement.h>

int counter = 0;

void setup() {

  /*  If you need any peripheral while sleep, remove below line.
   *  But it makes Ameba save less power (around 5.5 mA). */
  PowerManagement.setPllReserved(false);

  /* Make Ameba automatically suspend and resume while there is no on-going task. */
  PowerManagement.sleep();
}

void loop() {
  Serial.print("Keep running, counter: ");
  Serial.println(counter++);
  delay(1000);
}