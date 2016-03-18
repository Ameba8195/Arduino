/*
  This sketch use mobile phone to control car with streaming video.

  At first Ameba connect to AP.
  Then Ameba open UVC service.
  After UVC service is enabled, user can use rtsp player and open streaming with address:
    rtsp://192.168.1.123/test.sdp
  (NOTE: the IP address depends on Ameba's IP")

  For car related control method, please refer example car2wd_mobile_control
*/

#include <WiFi.h>
#include <Car2wd.h>
#include <UVC.h>

char ssid[] = "mycar";        // Set the AP's SSID
char pass[] = "12345678";     // Set the AP's password
char channel[] = "6";         // Set the AP's channel
int status = WL_IDLE_STATUS;  // the Wifi radio's status

WiFiServer server(5001);

Car2wd car(8,9,10,11,12,13);

void setup() {
  car.begin();

  status = WiFi.status();

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.apbegin(ssid, pass, channel);
    if (status == WL_CONNECTED) {
      break;
    }
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("AP mode already started");

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

  server.begin();
}

uint8_t buffer[256];
void loop() {
  WiFiClient client = server.available();
  while (client.connected()) {
    if (client.read(buffer, sizeof(buffer)) > 0) {
      handleData((const char *)buffer);
    }
  }
  delay(1000);
}

int speedMapping(int rawspeed) {
  /* Original data range: -12 ~ + 12
   * Convert it to PWM range: 150~255
   **/
  if (rawspeed == 0) {
    return 0;
  } else {
    return (rawspeed * 105) / 12 + ((rawspeed > 0) ? 150 : -150);
  }
}

int lastXspeed = 0;
int lastYspeed = 0;
void handleData(const char *buf) {

  int xspeed;
  int yspeed;

  if (buf[0] == 'X') {
    xspeed = lastXspeed = speedMapping(atoi(&(buf[2])));
    yspeed = lastYspeed;
  } else if (buf[0] == 'Y') {
    xspeed = lastXspeed;
    yspeed = lastYspeed = speedMapping(atoi(&(buf[2])));
  }

  if (xspeed == 0 && yspeed == 0) {
    car.stop();
  } else if (xspeed == 0 && yspeed > 0) {
    car.setAction(CAR_FORWARD, abs(yspeed));
  } else if (xspeed == 0 && yspeed < 0) {
    car.setAction(CAR_BACKWARD, abs(yspeed));
  } else if (xspeed > 0 && yspeed < 0 ) {
    car.setAction(CAR_BACKWARD_RIGHT, abs(yspeed));
  } else if (xspeed > 0 && yspeed == 0) {
    car.setAction(CAR_ROTATE_RIGHT, abs(xspeed));
  } else if (xspeed > 0 && yspeed > 0 ) {
    car.setAction(CAR_FORWARD_RIGHT, abs(yspeed));
  } else if (xspeed < 0 && yspeed < 0 ) {
    car.setAction(CAR_BACKWARD_LEFT, abs(yspeed));
  } else if (xspeed < 0 && yspeed == 0) {
    car.setAction(CAR_ROTATE_LEFT, abs(xspeed));
  } else if (xspeed < 0 && yspeed > 0 ) {
    car.setAction(CAR_FORWARD_LEFT, abs(yspeed));
  }
}