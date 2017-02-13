/*
 * Demonstrates use of deepsleep api of power management
 *
 * This sketch make a delay for 30s before enter deepsleep.
 * if Ameba is under deepsleep, it's not able to upload image.
 * Without this delay, you will find it's hard to upload image 
 * because Ameba is always under deepsleep.
 *
 * If you accidentally put deepsleep at the beginning,
 * you can connect D18(For RTL8710, it's D15) to GND. It's a software lock of power save
 * which prevent Ameba enter any power save state.
 *
 * When Ameba resume from deepsleep, it starts from beginning 
 * instead of original program position. It's just like it is been 
 * through a reset.
 *
 */

#include <PowerManagement.h>

void setup() {

  /*  Make a safe delay before enter deepsleep
   *  if Ameba is under deepsleep, it's not able to upload image.
   */
  delay(30000);

  Serial.println("Enter deepsleep for 10s");
  PowerManagement.deepsleep(10000);

  /*  After Ameba resume from deepsleep, 
   *  it starts from beginning just like it's been through a reset.
   */
}

void loop() {
  Serial.println("You won't see this log");
  delay(1000);
}