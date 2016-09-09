/*

In this sketch, Ameba would perform OTA via invoke update_ota_local.
It is executed in non-block mode and try to connect to OTA TCP server then download image.
After verified this image Ameba would reboot and run this image.

*/

#include <WiFi.h>

#define OTA_SERVER "192.168.1.19"
#define OTA_PORT   8082

char ssid[] = "yourNetwork"; //  your network SSID (name)
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

WiFiClient client;

void setup() {

  Serial.println("This is version 1");

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("retry in 5s");
    delay(5000);
  }
  Serial.println("Connected to wifi");

  /*
   * update_ota_local executes in non-block mode. 
   * it will try connecting to server, and download the image.
   * After verified this image, the system would reboot and run this image.
   */
  update_ota_local(OTA_SERVER, OTA_PORT);
}

void loop() {
  delay(1000);
}