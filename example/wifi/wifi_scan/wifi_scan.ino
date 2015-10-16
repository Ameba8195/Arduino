#include "wifi1.h"

void setup() {
  // initialize serial and wait for the port to open:
  Serial.begin(9600);
  while(!Serial) ;

  Serial.println("sleep 5 sec. ");
  delay(5000);

  // attempt to connect using WEP encryption:
  Serial.println("Initializing Wifi...");
  printMacAddress();

  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
}

void loop() {
  delay(10000);
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
}

void printMacAddress() {
  // the MAC address of your Wifi shield
  byte mac[6];                     

  // print your MAC address:
  WiFi1.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void show_enc_type(uint8_t type) {
  switch (type) {
    case 2 : 
        Serial.print(" TKIP ");
        break;
    case 4 : 
        Serial.print(" CCMP ");
        break;
    case 5 : 
        Serial.print(" WEP ");
        break;
    case 7 : 
        Serial.print(" None ");
        break;
    case 8 : 
        Serial.print(" Auto ");
        break;
    default : 
        Serial.print(" wrong ttype ");
        break;
  }
}


void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi1.scanNetworks();

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);


  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi1.SSID(thisNet));

    Serial.print("\tSignal: ");
    Serial.print(WiFi1.RSSI(thisNet));
    Serial.println(" dBm");

    Serial.print("\tEncryption: ");
    show_enc_type(WiFi1.encryptionType(thisNet));
    
    Serial.println(" ");
  }
 
}

