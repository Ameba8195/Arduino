#include <WiFi.h>
#include <HttpClient.h>
#include <Xively.h>

//WiFi related setting
char ssid[] = "YourNetwork";   //your network SSID (name) 
char pass[] = "password";     // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;             // your network key Index number (needed only for WEP)

//Xively related setting
#define xivelyFeed YourFeedID     //your xively feed ID
char xivelyKey[] = "YourAPIKEY";  //your xively API KEY
char sensorId[] = "sensor_reading"; //Define the strings for our datastream IDs
int datastream_number = 1;          //the number of datastreams

//define a peseudo data
int sensorValue = 1;

XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
};
XivelyFeed feed(xivelyFeed, datastreams, datastream_number);
int status = WL_IDLE_STATUS;
WiFiClient client;
XivelyClient xivelyclient(client);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.println("Reading from Xively example");
  Serial.println();

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void loop() {
  // put your main code here, to run repeatedly:
  datastreams[0].setFloat(sensorValue);
  sensorValue++; //updating the pesuedo data
  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());

  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  delay(15000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
