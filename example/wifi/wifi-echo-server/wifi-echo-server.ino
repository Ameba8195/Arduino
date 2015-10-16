/*
 
 This example connects to an unencrypted Wifi network. 
 Then it prints the  MAC address of the Wifi shield,
 the IP address obtained, and other network details.

 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
 #include <WiFi1.h>
 #include "WiFiServer1.h"

char ssid[] = "YourAPName";     // the name of your network
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiServer1 server(7);

void setup() {
  Serial.begin(38400);
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi1.begin(ssid);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  } 
  else {
    Serial.println("Connected to wifi, delay 3 sec for DHCP");
    delay(3000);
    IPAddress ip = WiFi1.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    Serial.println("Server begin");
    server.begin();
     
  }   
  
}

char buffer[256];

void loop() {
  // check the network connection once every 10 seconds:
  // listen for incoming clients
  WiFiClient1 client = server.available();
  
  Serial.print("Connection from: ");
  Serial.println(client.get_address());

     while (true) {
        int n = client.read((uint8_t*)(&buffer[0]), sizeof(buffer));
        if (n <= 0) break;
        
        n = server.write(buffer, n);
        if (n <= 0) break;
    }
        
    client.stop();
}


