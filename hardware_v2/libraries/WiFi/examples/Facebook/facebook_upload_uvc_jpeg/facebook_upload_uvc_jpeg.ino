/*
  In this sketch, it assumed you already have a user access token with "publish_actions" scopes.
  Using this token you can get a jpeg from a uvc and post it into facebook feed.

  If you never use facebook graph api, please refer below tutorial as warm up:
    https://www.amebaiot.com/ameba-arduino-facebook/

  If you don't have access token, please reference below to get one:
    https://developers.facebook.com/tools/explorer/
  And you need these access right to publish on feed:
    https://developers.facebook.com/docs/graph-api/reference/v2.8/user/feed

  If you already have one, using link blow to check the access right:
    https://developers.facebook.com/tools/debug/accesstoken
  You can also extend the expire time of access token in this page.
*/

#include <WiFi.h>
#include <UVC.h>

char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "secretPassword";  // your network password
int keyIndex = 0;                // your network key Index number (needed only for WEP)

char server[] = "graph.facebook.com";    // facebook graph api service

char access_token[] = "EAAEcZAcqQqtUBAPsfzwk1KxTNv2WwaasZAVvcqPXeAUwssSfUhZBen4I65BIUY7gedZC63V3wos0JFYZCcp26ZCXVwO3cZAGAeuZAOvozhBTZBoDUUZCqUCPGZARbP8gFYDQsGPz2tO8ctpTxnqCrN3YbPFyGswbhclOECXrzj1vFzstxGd2Y2ezslpKVdOYa19rp7DOHR0miNk3QZDZD";

WiFiSSLClient client;
int chunkSize = 1446;

#define BOUNDARY "-----boundary12345"

char data_file_header[] = \
  "--" BOUNDARY "\r\n" \
  "Content-Disposition: form-data; name=\"source\"; filename=\"test.jpg\"\r\n" \
  "Content-Type: application/octet-stream\r\n\r\n";

char data_end[] = "\r\n--" BOUNDARY "--\r\n";

unsigned char jpegbuf[40 * 1024];

void setup() {
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");

  UVC.begin(UVC_MJPEG, 320, 240, 30, 0, JPEG_CAPTURE);

  // wait until UVC is ready for streaming
  while (!UVC.available()) {
    delay(100);
  }
  delay(1000);
  Serial.println("UVC is ready");
}

void loop() {
  int i;
  int len = UVC.getJPEG(jpegbuf);
  int contentlen = strlen(data_file_header) + len + strlen(data_end);
  if (len > 0) {
    while (1) {
      if (!client.connect(server, 443)) {
        Serial.println("Connect to server failed. Retry after 1s.");
        client.stop();
        delay(1000);
        continue;
      }
  
      Serial.println("connected to server");
      // Make a HTTP request:
      client.print("POST /v2.9/me/photos?access_token=");
      client.print(access_token);
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(server);
      client.println("Content-Type: multipart/form-data; boundary=" BOUNDARY);
      client.print("Content-Length: ");
      client.println(contentlen);
      client.println();

      // upload photo as file
      client.write(data_file_header, strlen(data_file_header));
      for (i = 0; i < len; i += chunkSize) {
        client.write(jpegbuf + i, (i+chunkSize <= len) ? chunkSize : len - i);
      }
      client.write(data_end, strlen(data_end));
      Serial.println("photo has been uploaded");

      // try to get response
      while (!client.available()) delay(100);
      while (client.available()) {
        char c = client.read();
        Serial.write(c);
      }
      client.stop();
      break;
    }
  }
  delay(60 * 1000);
}