/*
 * Demonstrates use of deepsleep api of power management with DHTxx and wifi
 *
 * This sketch make wifi connection, read DHTxx results, send out
 * results, and the make a deepsleep or delay for 60s.
 *
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <PowerManagement.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// Update these with values suitable for your network.
char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "secretPassword";  // your network password
int status  = WL_IDLE_STATUS;    // the Wifi radio's status

char udpSrvIp[] = "192.168.1.1"; // the received UDP server IP
int udpSrvPort = 2390;           // the received UDP server port

char databuf[64];

WiFiUDP Udp;

void setup()
{
  Serial.begin(38400);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    if (status == WL_CONNECTED) {
      break;
    }
    // wait 10 seconds for retry:
    delay(10000);
  }

  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  while (isnan(h) || isnan(t)) {
    Serial.println("DHTxx retry");
    delay(2000);
    h = dht.readHumidity();
    t = dht.readTemperature();
  }

  sprintf(databuf, "h: %f %%\tt: %f *C", h, t);

  Udp.beginPacket(udpSrvIp, udpSrvPort);
  Udp.write(databuf);
  Udp.endPacket();

  if (!PowerManagement.safeLock()) {
    Serial.println("deepsleep 60s");
    PowerManagement.deepsleep(60000);

    Serial.println("You won't see this log");
    delay(1000);
  } else {
    /* As D18 is connected to GND, a software safe lock prevents Ameba enter power save */
    Serial.println("delay 60s");
    delay(60000);
  }
}
