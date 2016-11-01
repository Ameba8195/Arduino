/*
 * Demonstrates use of deepsleep api of power management with DHTxx series
 *
 * If we have a period task and do not need continuous wifi connection,
 * we can replace the original delay() of loop() with deepsleep.
 *
 * This sketch is modified from example "DHTTester.ino".
 * We measure humidity and temperature when Ameba startup, then enter deepsleep 
 * for 60s. The whole behavior is similar original example, except that Ameba
 * is under deepsleep between 2 measurements.
 *
 * Please refer example "DHT11Tester" for DHT11 pin connection
 **/

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <PowerManagement.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.println("DHTxx test!");

  dht.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  PowerManagement.deepsleep(60000);

  Serial.println("You won't see this log");
  delay(1000);
}
