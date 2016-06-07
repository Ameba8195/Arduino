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

#ifndef	LCD12864_h
#define LCD12864_h
#include <avr/pgmspace.h>
#include <inttypes.h>


class LCD12864 {
typedef unsigned char byte;



public:

LCD12864();

 void Initialise(void);
 void selectCS1(void);
void setPins(uint8_t tRS, uint8_t tRW, uint8_t tD7, uint8_t tD6, uint8_t tD5, uint8_t tD4, uint8_t tD3, uint8_t tD2, uint8_t tD1, uint8_t tD0);
void delayns(void);
void VectorConverter(uint8_t vector);
void VectorConvertermany(uint8_t vector, uint8_t amount);

void Render(void);
void DrawPixel(uint8_t startX, uint8_t startY);
void DrawBrLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius);
void DrawRectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height);
void FillRectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height);

void DrawScreenBuffer(uint8_t X,uint8_t Y);
void Duplicate(bool x);
void Draw(bool t, uint8_t x, uint8_t y);
void setdelay(uint8_t newtime);
void DrawSprite(uint8_t xPos, uint8_t yPos);

void RenderScreenBuffer(uint8_t screen);
void RenderScreenBuffer(uint8_t startX, uint8_t startY, uint8_t maxX, uint8_t maxY);


void DumpScreenBuffer(void);
void DumpScreenBuffer(uint8_t startX, uint8_t startY, uint8_t maxX, uint8_t maxY);


byte ScreenBuffer[16][32];


byte checkAND[8];


byte delaytime;
byte DEFAULTTIME;

static const byte RS = 17; 
static const byte RW = 16;  
static const byte EN = 18;  
static const byte D0  = 8;  
static const byte D1  = 9; 
static const byte D2  = 10;  
static const byte D3  = 11;  
static const byte D4  = 4; 
static const byte D5  = 5;  
static const byte D6  = 6;  
static const byte D7  = 7; 
static const byte CSEL1  = 14; 



byte currentXpos;
byte currentYpos;

byte temp[8];


};
extern LCD12864 LCDA;    
#endif
