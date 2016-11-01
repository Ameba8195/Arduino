/*
 Basic Microsoft Azure IoT Hub example

*/

#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

char devinfoStr[] = "{ \"ObjectType\":\"DeviceInfo\", \"Version\":\"1.0\", \"IsSimulatedDevice\":false, \"DeviceProperties\":\"DeviceProperties\",  \"Commands\": [ {\"Name\":\"SetHumidity\", \"Parameters\":[{\"Name\":\"humidity\",\"Type\":\"double\"}]}, { \"Name\":\"SetTemperature\", \"Parameters\":[{\"Name\":\"temperature\",\"Type\":\"double\"}]} ] }";
char devicePropertiesStr[] = "{ \"DeviceID\":\"device\", \"HubEnabledState\":true, \"Manufacturer\": \"manufacturer\", \"Latitude\": 43.7184039, \"Longitude\": -79.5181426}";

// WiFi settings
char ssid[]           = "ssid"; //  your network SSID (name)
char password[]       = "password";  // your network password

// MQTT settings
char mqttServer[]     = "AmebaGO.azure-devices.net";
uint16_t  mqttPort         = 8883;
char mqttClientName[] = "AmebaGO1";
char mqttUsername[]   = "AmebaGO.azure-devices.net/AmebaGO1" ;       // your MQTT username
char mqttPassword[]   = "SharedAccessSignature sr=AmebaGO.azure-devices.net%2fdevices%2fAmebaGO1&sig=YuJQ06ImZh0AJD7vtc00ZD7tNPBtI9v8m%2fX4tKTirkE%3d&se=1504272564"; //SAS token, your MQTT password
char mqttTopic[]      = "devices/AmebaGO1/messages/devicebound/#";  // your MQTT topic 
char mqttPublishEvents[] = "devices/AmebaGO1/messages/events/";

//MQTT Devices settings
char manufacturer[] = "Realtek";
double latitude = 43.7184039;
double longitude = -79.5181426;

// variables will change:
int wifiStatus        = WL_IDLE_STATUS;  // the Wifi radio's status

// Initialize the WiFi SSL client library
WiFiSSLClient wifiSSLClient;


// Initialize the PubSubClient
//PubSubClient mqttClient(wifiSSLClient, mqttServer, mqttPort);
PubSubClient mqttClient(wifiSSLClient);

void setupWiFi();
char buffer[500];

void callback(char* topic, byte* payload, unsigned int length) {
  
  char buf[MQTT_MAX_PACKET_SIZE];  
  strncpy(buf, (const char *)payload, length);
  buf[length] = '\0';
  
  Serial.print(F("Message arrived: "));
  Serial.println(topic);
  Serial.println( buf);
  //printf("Message arrived [%s] %s\r\n", mqttTopic, buf);
  
}

char * serializeData(){
  
  StaticJsonBuffer<JSON_OBJECT_SIZE(16)> jsonBuffer;  
  JsonObject& publishData = jsonBuffer.createObject();

  publishData["DeviceId"] = mqttClientName;
  publishData["Temperature"] = 50;
  publishData["Humidity"] = 50;
  publishData["ExternalTemperature"] = 55;
  //publishData.prettyPrintTo(Serial);  //show the sensor data publish message
  publishData.printTo(buffer, sizeof(buffer));

  return buffer;
}

void setup() {
  

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  setupWiFi();  
  
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setPublishQos(MQTTQOS1);
  mqttClient.setCallback(callback);
  mqttClient.waitForAck(true);

  // Allow the hardware to sort itself out
  delay(1500);
  Serial.println(F("Connecting to MQTT broker ..."));

  while (true){
      if (mqttClient.connect(mqttClientName, mqttUsername, mqttPassword))       
      {
        Serial.println(F("Connected"));        
        mqttClient.subscribe(mqttTopic);
        break;
      } else {
        Serial.print(F("Connection failed: "));
        Serial.println(mqttClient.state());
        delay(1000);        
      }
  }

  StaticJsonBuffer<150> devPropBuffer;
  JsonObject& deviceProperties = devPropBuffer.parseObject(devicePropertiesStr);
  if (!deviceProperties.success()) {
    Serial.println("deviceProperties parseObject() failed");
    return;
  }

  StaticJsonBuffer<500> devinfoBuffer;
  JsonObject& devinfo = devinfoBuffer.parseObject(devinfoStr);
  if (!devinfo.success()) {
    Serial.println("devinfo parseObject() failed");
    return;
  }  
  
  deviceProperties["DeviceID"] = mqttClientName;
  deviceProperties["Manufacturer"] = manufacturer;
  deviceProperties["Latitude"] = double_with_n_digits(latitude, 7);
  deviceProperties["Longitude"] = double_with_n_digits(longitude, 7);  
  devinfo["DeviceProperties"] = deviceProperties;
  //devinfo.prettyPrintTo(Serial);  //show the devinfo publish message

  devinfo.printTo(buffer, sizeof(buffer));
  //Serial.println(buffer);
  Serial.println(F("Devinfo Publishing..."));
  while (!mqttClient.publish(mqttPublishEvents, buffer))  
  {
      Serial.print("client.publish FAILED:");
      Serial.println(mqttClient.state());
      delay(1000);
  }
      Serial.println(F("Devinfo Publish done"));
  
  
}

void setupWiFi() {

  // attempt to connect to Wifi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    wifiStatus = WiFi.begin(ssid, password);

    if (wifiStatus != WL_CONNECTED) {
      // wait 10 seconds for next connection attempt
      delay(10000);
    }
  }
  Serial.println("");
  Serial.println(F("Connected to wifi"));

  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  Serial.print(F("signal strength (RSSI):"));
  Serial.print(WiFi.RSSI());
  Serial.println(F(" dBm"));
}

void loop() {
  
  Serial.print(serializeData());
  Serial.println(F("Messages Publishing..."));
  if (!mqttClient.publish(mqttPublishEvents, serializeData()))
  {
      Serial.print("client.publish FAILED:");
      Serial.println(mqttClient.state());
  }else{
      Serial.println(F("Messages Publish done"));
  }
  
  // let the MQTT client process events
  mqttClient.loop();
  delay(10000);
  
}

