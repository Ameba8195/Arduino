/*
 * This sketch shows how to take jpeg capture from UVC and send out to remote server
 *
 * You need to fill in remote server's address and port.
 * Ex. If remote server is linux system, then you can use netcat tool to capture the jpeg file:
 *     nc -l 5001 > my_jpeg_file.jpeg
 *
 */

#include <WiFi.h>
#include <UVC.h>

char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "password";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned char jpegbuf[40 * 1024];

WiFiClient client;
char serverIP[] = "192.168.1.65"; // The remote server IP to receive jpeg file
int  serverPort = 5001;           // the remote server port
int chunkSize = 1460;             // If MTU=1500, then data payload = 1500 - IPv4 header(20) - TCP header(20) = 1460

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
    // wait 1 seconds for retry
    delay(1000);
  }
  Serial.print("You're connected to the network");

  UVC.begin(UVC_MJPEG, 640, 480, 30, 0, JPEG_CAPTURE);

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
  if (len > 0) {
    if (client.connect(serverIP, serverPort)) {
      for (i = 0; i < len; i += chunkSize) {
        if (i+chunkSize <= len) {
          client.write(jpegbuf+i, chunkSize);
        } else {
          client.write(jpegbuf+i, len - i);
        }
      }
      client.stop();
      Serial.println("Send out file");
    } else {
      Serial.println("Fail to connect");
    }
  } else {
    Serial.println("Fail to get jpeg");
  }
  delay(1000);
}