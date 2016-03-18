/*
  This sketch demonstrate how to streaming video of usb camera.
  Ameba receive video data from usb port and streaming out on wifi with rtsp protocol.
  User can receive ths streaming data on PC or laptop with rtsp player.

  In this sketch you need:
    Ameba x 1
    usb camera x 1
    OTG wire x 1 (with extra power line on OTG if needed)

  At first Ameba connect to AP.
  Then Ameba open UVC service.
  After UVC service is enabled, user can use rtsp player and open streaming with address:
    rtsp://192.168.1.123/test.sdp
  (NOTE: the IP address depends on Ameba's IP")

*/

#include <WiFi.h>
#include <UVC.h>

char ssid[] = "yourNetwork";     //  your network SSID (name)
char pass[] = "secretPassword";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void setup() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) {
      break;
    }
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.print("You're connected to the network");

  // Default setting is motion jpeg with 640x480 resolution and frame rate is 30fps
  UVC.begin();
  // Try below setting if you found the stream quality is poor
  //UVC.begin(UVC_MJPEG, 320, 240, 30, 0);

  // wait until UVC is ready for streaming
  while (!UVC.available()) {
    delay(100);
  }
  Serial.println("UVC is ready");
  Serial.println("Open your rtsp player with this address:");
  Serial.print("\trtsp://");
  Serial.print(WiFi.localIP());
  Serial.println("/test.sdp");
}

void loop() {
  delay(10000);
}