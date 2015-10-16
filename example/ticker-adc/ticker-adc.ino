#include "arduino.h"
#include "DAC1.h"
#include "Ticker.h"

#define LED1  13

static uint16_t buffer[100];
static float voltage=0.1;
static float voltage1;
static uint32_t temp;

Ticker flipper;

void flip()  {
    digitalWrite(LED1, !digitalRead(LED1));
    
    // read from ADC
    temp = analogRead(2);
    Serial.print("ADC read=");
    Serial.print(temp);

    // arduino : 5V - 1023    
    voltage1 = (float)temp*5/(float)(1024);
    Serial.print(", voltage=");
    Serial.println(voltage1);
}

void setup() {
    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, HIGH);
    flipper.attach(&flip, 1);
  
    Serial.println("DAC start");
    DAC0.begin(DAC_DATA_RATE_10K);
    
}


void loop() {
   // DAC output
    voltage+=0.1;
    if (voltage>3.0) voltage=0.1;
    //Serial.print("voltage=");
    //Serial.println(voltage);
    
    for (int i=0; i<100; i++ )
        buffer[i] = (unsigned int)(voltage*(float)(0xFFF)/3.3);      

    DAC0.send16_repeat((uint16_t *)&buffer[0], 100, 100);
    
    delay(1000);
}

