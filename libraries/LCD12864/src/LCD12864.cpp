/*
LCD12864
Created by Markos Kyritsis
This code is completely open source, and you may edit it however you like. 
Please support the Arduino community, and upload any bug fixes to the relative
page. 
Keep in mind that the code is for demonstration purposes only, I can't be
held responsible if your device suddenly zaps you into millions of little
atoms, and you live the rest of your existence in a quantum universe where
nothing makes sense anymore (you know, you exist in more than one place, changing
between matter and waves, while at the same time half your electrons are entangled
in some other point in space and time). 
Other than that, enjoy... =D
*/

#include "LCD12864.h"
#include "LCDSprite.h"
#include "Arduino.h"

extern "C" {
#include <wiring.h> 
#include <inttypes.h>
#include <avr/pgmspace.h>
}


LCD12864::LCD12864() {

this->DEFAULTTIME = 80; // 80 ms default time
this->delaytime = DEFAULTTIME;
}

/*
void LCD12864::configPins(uint8_t tCSEL1, uint8_t tCSEL2, uint8_t tEN, uint8_t tRS, uint8_t tRW, uint8_t tD7, uint8_t tD6, uint8_t tD5, uint8_t tD4, uint8_t tD3, uint8_t tD2, uint8_t tD1, uint8_t tD0) {

this->EN = tEN;
this->RS = tRS;
this->RW = tRW;
this->D7 = tD7;
this->D6 = tD6;
this->D5 = tD5;
this->D4 = tD4;
this->D3 = tD3;
this->D2 = tD2;
this->D1 = tD1;
this->D0 = tD0;
this->CSEL1 = tCSEL1; 
this->CSEL2 = tCSEL2; 
}
*/
void LCD12864::setdelay(uint8_t newtime) {
this->delaytime = newtime;
}
/*
void LCD12864::configPins() {
this->EN = 18;
this->RS = 17;
this->RW = 16;
this->D7 = 7;
this->D6 = 6;
this->D5 = 5;
this->D4 = 4;
this->D3 = 11;
this->D2 = 10;
this->D1 = 9;
this->D0 = 8;
this->CSEL1 = 14; 
}
*/




void LCD12864::selectCS1(void) {
  digitalWrite(this->EN, 1);   
delayns();
  digitalWrite(this->CSEL1, 1);   
delayns();
  digitalWrite(this->EN, 0);   
}

void LCD12864::Initialise(void) {
this->checkAND[7]= B00000001;
this->checkAND[6]   = B00000010;
this->checkAND[5] = B00000100;
this->checkAND[4]  = B00001000;
this->checkAND[3]  = B00010000;
this->checkAND[2]  = B00100000;
this->checkAND[1]  = B01000000;
this->checkAND[0]  = B10000000;


   pinMode(CSEL1, OUTPUT);    
//   pinMode(this->CSEL2, OUTPUT);    
   pinMode(RS, OUTPUT);     
   pinMode(RW, OUTPUT);    
   pinMode(EN, OUTPUT);     
   pinMode(D0, OUTPUT);      
   pinMode(D1, OUTPUT);    
   pinMode(D2, OUTPUT);   
   pinMode(D3, OUTPUT);      
   pinMode(D4, OUTPUT);      
   pinMode(D5, OUTPUT);      
   pinMode(D6, OUTPUT);      
   pinMode(D7, OUTPUT);      

delayns();
selectCS1();
delayns();



  // 5FH
 setPins(0,0,0,1,0,1,1,1,1,1);
  // 34H
  setPins(0,0,0,0,1,1,0,1,0,0);
//30H
setPins(0,0,0,0,1,1,0,0,0,0);

//01h
setPins(0,0,0,0,0,0,0,0,0,1);

//06h
setPins(0,0,0,0,0,0,0,1,1,0);

//0ch
setPins(0,0,0,0,0,0,1,1,0,0);

//try for gfx
setPins(0,0,1,0,1,1,1,1,1,1);



delayns();
delayns();


 setPins(0,0,0,0,1,1,0,1,0,0); // 8BIT Mode-4 ext gfx mode
setPins(0,0,0,0,0,0,0,0,1,0); // Vertical scroll
//  setPins(0,0,1,0,0,0,0,0,0,0); // SET 0,0

delayns();
delayns();


setPins(0,0,1,0,0,0,0,0,0,0); // SET DDRAM Y
setPins(0,0,1,0,0,0,0,0,0,0); // SET DDRAM X

// Clear the Screen



for (int j = 128; j <= 160; j++) {
for (int i = 0; i <= 256; i+=8) {
  setPins(1,0,0,0,0,0,0,0,0,0);
}

VectorConverter(j); // lets get the binary from the dec
setPins(0,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // SET DDRAM Y
setPins(0,0,1,0,0,0,0,0,0,0); // SET DDRAM X


}

this->currentXpos = 0; // Starting coordinate lEFT of screen
this->currentYpos = 128; // // Starting coordinate TOP of screen



}




void LCD12864::VectorConverter(uint8_t vector) {
int result = vector;
for (int i = 7; i >= 0; i--) {
this->temp[i] = result % 2;

result = result/2;
}

}

void LCD12864::VectorConvertermany(uint8_t vector,uint8_t amount) {
int result = vector;
for (int i = amount-1; i >= 0; i--) {
this->temp[i] = result % 2;

result = result/2;
}

}


void LCD12864::Render(void) {
  setPins(0,0,0,0,1,1,0,1,1,0); // 8BIT Mode-4 ext gfx mode
}
void LCD12864::Draw(bool t, uint8_t x, uint8_t y) {
if (t == true) // Graphic Select
  setPins(0,0,0,0,1,1,0,1,1,0); // 8BIT Mode-4 ext gfx mode
else { // Text select, are all these really necessary? I really don't know

x += 128;
y += 128;
  // 5FH
// setPins(0,0,0,1,0,1,1,1,1,1);
  // 34H
  setPins(0,0,0,0,1,1,0,1,0,0);
//30H
setPins(0,0,0,0,1,1,0,0,0,0);

//01h
setPins(0,0,0,0,0,0,0,0,0,1);

//06h
setPins(0,0,0,0,0,0,0,1,1,0);

//0ch
setPins(0,0,0,0,0,0,1,1,0,0);

VectorConverter(y); // lets get the binary from the dec
setPins(0,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // SET DDRAM Y
VectorConverter(x); // lets get the binary from the dec
setPins(0,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // SET DDRAM X

/*
//Move Y
setPins(0,0,1,0,0,0,0,0,0,0);
//Move X
setPins(0,0,1,0,0,0,0,1,0,0);
*/
}

}


void LCD12864::FillRectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height)
{
for (uint8_t i = y0+1; i < y0+height; i++) {
DrawBrLine(x0, i, x0+width, i);
}

}

void LCD12864::DrawRectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height)
{
// Draw Top side
DrawBrLine(x0, y0, x0+width+1, y0);

// Draw Bottom side 
DrawBrLine(x0, y0+height, x0+width+1, y0+height);

// Now the sides
for (uint8_t i = y0+1; i < y0+height; i++) {
DrawPixel(x0,i);
DrawPixel(x0+width,i);
}





}
void LCD12864::DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius)  //Midpouint8_t circle algorithm
  {
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
 
    DrawPixel(x0, y0 + radius);
    DrawPixel(x0, y0 - radius);
    DrawPixel(x0 + radius, y0);
    DrawPixel(x0 - radius, y0);
 
    while(x < y)
    {
      if(f >= 0) 
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;    
      DrawPixel(x0 + x, y0 + y);
      DrawPixel(x0 - x, y0 + y);
      DrawPixel(x0 + x, y0 - y);
      DrawPixel(x0 - x, y0 - y);
      DrawPixel(x0 + y, y0 + x);
      DrawPixel(x0 - y, y0 + x);
      DrawPixel(x0 + y, y0 - x);
      DrawPixel(x0 - y, y0 - x);
    }
}



void  LCD12864::DrawBrLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) { // Bresenham Line Algorithm
   int Dx = x1 - x0; 
   int Dy = y1 - y0;
   int steep = (abs(Dy) >= abs(Dx));
   if (steep) {

int a = x0;
int b = y0;
x0=b;
y0=a;

 a = x1;
 b = y1;
x1=b;
y1=a;

       // recompute Dx, Dy after swap
       Dx = x1 - x0;
       Dy = y1 - y0;
   }
   int xstep = 1;
   if (Dx < 0) {
       xstep = -1;
       Dx = -Dx;
   }
   int ystep = 1;
   if (Dy < 0) {
       ystep = -1;		
       Dy = -Dy; 
   }
   int TwoDy = 2*Dy; 
   int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
   int E = TwoDy - Dx; //2*Dy - Dx
   int y = y0;
   int xDraw, yDraw;	
   for (int x = x0; x != x1; x += xstep) {		
       if (steep) {			
           xDraw = y;
           yDraw = x;
       } else {			
           xDraw = x;
           yDraw = y;
       }
       // plot
       DrawPixel(xDraw, yDraw);
       // next
       if (E > 0) {
           E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
           y = y + ystep;
       } else {
           E += TwoDy; //E += 2*Dy;
       }
   }
}



void LCD12864::DrawScreenBuffer(uint8_t X, uint8_t Y) {

int xpos = X/8;
int ypos = (Y-128);

int leftX = X%8;
int orit = checkAND[leftX];

ScreenBuffer[xpos][ypos] |= orit;

}

void LCD12864::DrawSprite(uint8_t xPos, uint8_t yPos) {
int count = 0;
for (int j = 0; j < sheight; j++) {
for (int i = 0; i < swidth; i++) {
if (pgm_read_byte_near(header_data + count) == 1)
this->DrawScreenBuffer(xPos+i,yPos+128+j);
count++;
}
}
}




void LCD12864::RenderScreenBuffer(uint8_t startX, uint8_t startY, uint8_t maxX, uint8_t maxY) {
for (int k = startY; k < maxY; k++) {
VectorConverter(k+128); // Starting Y Position
setPins(0,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // SET DDRAM Y
setPins(0,0,1,0,0,0,0,0,0,0); // SET DDRAM X at start again


for (int i = 0; i < maxX; i++) {

// Check if a pixel exists
for (int j = 0; j < 8; j++) {
int res = ScreenBuffer[i][k] & checkAND[j];

if (res > 0) { // Looks like we found a 1
temp[j] = 1;
}
else
temp[j] = 0;
} // end j
setPins(1,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // draw remaining pixels =D
} // end i


} // end k

}


void LCD12864::RenderScreenBuffer(uint8_t screen) {

for (int k = 0; k < 32; k++) {
VectorConverter(k+128); // Starting Y Position
setPins(0,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // SET DDRAM Y
setPins(0,0,1,0,0,0,0,0,0,0); // SET DDRAM X at start again


if (screen == 2) {
for (int i = 0; i < 16; i++) {
setPins(1,0,0,0,0,0,0,0,0,0); // draw remaining pixels =D

}
}

for (int i = 0; i < 16; i++) {

// Check if a pixel exists
for (int j = 0; j < 8; j++) {
int res = ScreenBuffer[i][k] & checkAND[j];

if (res > 0) { // Looks like we found a 1
temp[j] = 1;
}
else
temp[j] = 0;
} // end j
setPins(1,0,this->temp[0],this->temp[1],this->temp[2],this->temp[3],this->temp[4],this->temp[5],this->temp[6],this->temp[7]); // draw remaining pixels =D
} // end i


} // end k

}


void LCD12864::DumpScreenBuffer() {
for (int j = 0; j < 32; j++) {
for (int i = 0; i < 16; i++) {

ScreenBuffer[i][j] = 0;
}
}
}

void LCD12864::DumpScreenBuffer(uint8_t startX, uint8_t startY, uint8_t maxX, uint8_t maxY) {
for (int j = startY; j < maxY; j++) {
for (int i = 0; i < maxX; i++) {

ScreenBuffer[i][j] = 0;
}
}
}

void LCD12864::DrawPixel(uint8_t startX, uint8_t startY) {
DrawScreenBuffer(startX,startY);

this->currentXpos = startX;
this->currentYpos = startY;
}






void LCD12864::Duplicate(bool x) {
if (x == true)
setPins(0,0,0,0,1,1,1,1,1,1); // Draw
else
setPins(0,0,0,0,1,1,1,1,1,0); // Draw

}

void LCD12864::setPins(uint8_t tRS, uint8_t tRW, uint8_t tD7, uint8_t tD6, uint8_t tD5, uint8_t tD4, uint8_t tD3, uint8_t tD2, uint8_t tD1, uint8_t tD0) {
digitalWrite(EN,1);  
delayns();

  digitalWrite(RS, tRS);   
  digitalWrite(RW, tRW);   
  digitalWrite(D7, tD7);   
  digitalWrite(D6, tD6);   
  digitalWrite(D5, tD5);   
  digitalWrite(D4, tD4);   
  digitalWrite(D3, tD3);   
  digitalWrite(D2, tD2);   
  digitalWrite(D1, tD1);   
  digitalWrite(D0, tD0);   
delayns();
  digitalWrite(EN, 0);   
delayns();

}

void LCD12864::delayns(void){   
delayMicroseconds(delaytime);
 }


LCD12864 LCDA = LCD12864();


