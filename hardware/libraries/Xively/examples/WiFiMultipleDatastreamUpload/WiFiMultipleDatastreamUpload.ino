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
int datastream_number = 3;          //the number of datastreams

// Define the strings for our datastream IDs
char sensorId[] = "sensor_reading";
char bufferId[] = "info_message";
String stringId("random_string");
const int bufferSize = 140;
char bufferValue[bufferSize]; // enough space to store the string we're going to send
XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  XivelyDatastream(bufferId, strlen(bufferId), DATASTREAM_BUFFER, bufferValue, bufferSize),
  XivelyDatastream(stringId, DATASTREAM_STRING)
};

//define a peseudo data
int sensorValue = 1;

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
  datastreams[0].setFloat(sensorValue);
  sensorValue++;
  
  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());

  datastreams[1].setBuffer("a message to upload");
  Serial.print("Setting buffer value to:\n    ");
  Serial.println(datastreams[1].getBuffer());

  // calculating a number to send up in a string
  String stringValue(sensorValue+100);
  stringValue += " is a number";
  datastreams[2].setString(stringValue);
  Serial.print("Setting string value to:\n    ");
  Serial.println(datastreams[2].getString());

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
