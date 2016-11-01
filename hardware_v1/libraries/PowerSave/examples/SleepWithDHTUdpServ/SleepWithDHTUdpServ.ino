/*
 * Demonstrates use of sleep api of power management with DHTxx and wifi
 *
 * This sketch keep wifi connection and open UDP server.
 * User can send UDP packet 'H' to query humidity, and send UDP packet 'T' to query temperature.
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

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[255]; // buffer to hold incoming packet
char ReplyBuffer[64];   // a string to send back

WiFiUDP Udp;

void setup()
{
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

  Udp.begin(localPort);

  /*  If you need any peripheral while sleep, remove below line.
   *  But it makes Ameba save less power (around 5.5 mA). */
  PowerManagement.setPllReserved(false);

  PowerManagement.sleep();

  dht.begin();
}

void loop()
{
  // read the packet into packetBufffer
  int len = Udp.read(packetBuffer, 255);

  if (len > 0) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    while (isnan(h) || isnan(t)) {
      Serial.println("DHTxx retry");
      delay(2000);
      h = dht.readHumidity();
      t = dht.readTemperature();
    }

    if (packetBuffer[0] == 'h' || packetBuffer[0] == 'H') {
      sprintf(ReplyBuffer, "H: %f %%", h);
    } else if (packetBuffer[0] == 't' || packetBuffer[0] == 'T') {
      sprintf(ReplyBuffer, "T: %f *C", t);
    }

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}
