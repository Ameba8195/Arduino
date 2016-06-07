#include <LCD12864.h>
#include <LCDSprite.h>

/*
 * 
 12864 Graphics LCD library
 
 12864 is an unofficial Arduino library that supports 12864 Graphic LCDs that use the ST7920 chip.
 
 Lets try to draw a Circle, and a the number 012 on screen
  
 * EN = Arduino Pin 18
 * RS = Arduino Pin 17
 * RW = Arduino Pin 16
 * D7 = Arduino Pin 7
 * D6 = Arduino Pin 6
 * D5 = Arduino Pin 5
 * D4 = Arduino Pin 4
 * D3 = Arduino Pin 11
 * D2 = Arduino Pin 10
 * D1 = Arduino Pin 9
 * D0 = Arduino Pin 8
 * CSEL1 = Arduino Pin 14
 You also need to connect a POT with 5V on one side, ground on the other, and 5V (perhaps with 150 Ohm resistance) + LCD Pin 19 to the wiper (middle one usually). Otherwise you won't see much.
 
 This example code is in the public domain.
 http://playground.arduino.cc/Code/LCD12864
 */

void setup() {
  // put your setup code here, to run once:
  LCDA.Initialise();
  delay(500);
  LCDA.Render();
  LCDA.DrawCircle(30,135,5);

  LCDA.RenderScreenBuffer(2); // lets draw it in the second screen
  LCDA.Draw(false,4,0);

  delay(1);

  LCDA.setPins(1,0,0,0,1,1,0,0,0,0);
  LCDA.setPins(1,0,0,0,1,1,0,0,0,1);
  LCDA.setPins(1,0,0,0,1,1,0,0,1,0);
  LCDA.Draw(true,4,0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
