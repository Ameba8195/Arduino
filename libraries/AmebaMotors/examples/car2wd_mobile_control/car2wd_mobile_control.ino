/*
 * This sketch use mobile phone to control car
 *
 * This sketch bring up wifi in AP mode.
 * User can change mobile phone's wifi AP with correspond ssid.
 * Device open TCP socket on port 5001, and mobile app connect to
 * it on default AP IP 192.168.1.1
 * Mobile app send raw data when control data is changed.
 * The Raw data has these format:
 *     <direction>:<speed>
 *
 *     Y:12
 *     It's forward with max speed. (Speed has range -12~12)
 *
 *     Y:-6
 *     It's backward with half speed.
 *
 *     X:12
 *     It's right rotate with max speed.
 *
 *     Y:12
 *     X:-6
 *     It's forward left with max speed. (When X and Y has speed value, use Y speed only)
 *     
 **/

#include <WiFi.h>
#include <Car2wd.h>

char ssid[] = "mycar";        // Set the AP's SSID
char pass[] = "12345678";     // Set the AP's password
char channel[] = "6";         // Set the AP's channel
int status = WL_IDLE_STATUS;  // the Wifi radio's status

WiFiServer server(5001);

Car2wd car(8,9,10,11,12,13);

void setup() {
  car.begin();

  status = WiFi.status();

  // attempt to start AP:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to start AP with SSID: ");
    Serial.println(ssid);
    status = WiFi.apbegin(ssid, pass, channel);
    if (status == WL_CONNECTED) {
      break;
    }
    delay(10000);
  }

  //AP MODE already started:
  Serial.println("AP mode already started");
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
