#include <WiFi.h>
#include <HttpClient.h>
#include <Xively.h>
#include <SoftwareSerial.h>

/*************************************WiFi related setting*********************************************/
char ssid[] = "YourNetwork";      //your network SSID (name) 
char pass[] = "password";         // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
/******************************************************************************************************/

/*************************************Xively device setting********************************************/
#define xivelyFeed YourFeedID     //your xively feed ID
char xivelyKey[] = "YourAPIKEY";  //your xively API KEY
/******************************************************************************************************/

/****************************Xively Channel for device's friendly name*********************************/
char device_mac[12];
String device_name = "pm_sensor_office";    //device's friendly name
/******************************************************************************************************/

/************************************Xively channel number*********************************************/
int datastream_number = 4;                  //the number of datastreams
/******************************************************************************************************/

/*******************************Xively channel for first data******************************************/
char data1[32];                 //The First data's name
char data1_type[] = "i";        //Data type("i" : integer; "f" : float; "s" : string; "t" : on/off
char data1_rw[] = "R";          //APP's permission type("R" : readonly; "W" : read and write 
char data1_name[] = "pm1.0";    //Displayed name on APP
/******************************************************************************************************/
/*******************************Xively channel for second data*****************************************/
char data2[32];                 //The second data's name
char data2_type[] = "i";        //Data type("i" : integer; "f" : float; "s" : string; "t" : on/off
char data2_rw[] = "R";          //APP's permission type("R" : readonly; "W" : read and write 
char data2_name[] = "pm2.5";    //Displayed name on APP
/******************************************************************************************************/
/*******************************Xively channel for third data******************************************/
char data3[32];                 //The third data's name
char data3_type[] = "i";        //Data type("i" : integer; "f" : float; "s" : string; "t" : on/off
char data3_rw[] = "R";          //APP's permission type("R" : readonly; "W" : read and write  
char data3_name[] = "pm10";     //Displayed name on APP
/******************************************************************************************************/

#if defined(BOARD_RTL8195A)
SoftwareSerial mySerial(0, 1); // RX, TX
#elif defined(BOARD_RTL8710)
SoftwareSerial mySerial(17, 5); // RX, TX
#else
SoftwareSerial mySerial(0, 1); // RX, TX
#endif

#define pmsDataLen 64
uint8_t buf[pmsDataLen];
int idx = 0;
int pm10 = 0;
int pm25 = 0;
int pm100 = 0;
int status = WL_IDLE_STATUS;
WiFiClient client;
XivelyClient xivelyclient(client);

void setup() {
  Serial.begin(57600);
  mySerial.begin(9600); // PMS 3003 UART has baud rate 9600

/***************************************Connect to Wifi network***************************************/
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid,pass);
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
/******************************************************************************************************/

/************************************Setting each channel's name***************************************/  
  byte mac[6];
  WiFi.macAddress(mac);

  Serial.println("Device Mac: ");
  sprintf(device_mac, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(device_mac);

  Serial.println("First data's Name: ");
  sprintf(data1, "%s_%s%s_%s", device_mac, data1_rw, data1_type, data1_name);
  Serial.println(data1);
  
  Serial.println("Second data's Name: ");
  sprintf(data2, "%s_%s%s_%s", device_mac, data2_rw, data2_type, data2_name);
  Serial.println(data2);

  Serial.println("Third data's Name: ");
  sprintf(data3, "%s_%s%s_%s", device_mac, data3_rw, data3_type, data3_name);
  Serial.println(data3);
/******************************************************************************************************/
}

void loop() {
/*******************************setting the all channels to the datastream*****************************/
  XivelyDatastream datastreams[] = {
    XivelyDatastream(device_mac, strlen(device_mac), DATASTREAM_STRING),
    XivelyDatastream(data1, strlen(data1), DATASTREAM_INT),
    XivelyDatastream(data2, strlen(data2), DATASTREAM_INT),
    XivelyDatastream(data3, strlen(data3), DATASTREAM_INT)
  };
/******************************************************************************************************/
  XivelyFeed feed(xivelyFeed, datastreams, datastream_number);
 
/***********************************getting the pm sensor's value**************************************/
  uint8_t c = 0;
  idx = 0;
  unsigned short checksum, calchecksum;
  memset(buf, 0, pmsDataLen);

  while (idx != pmsDataLen) {
    while(!mySerial.available());
    buf[idx++] = mySerial.read();
  }
      int i;
    for(i=0;i<63;i++){
      if(buf[i]==0x42 && buf[i+1]==0x4d)
        break;
    }
    if(i>=62){
      Serial.println("pms5003 cannot find start code\n");
      return;
    }

  pm10 = ( buf[i+10] << 8 ) | buf[i+11];
  pm25 = ( buf[i+12] << 8 ) | buf[i+13];
  pm100 = ( buf[i+14] << 8 ) | buf[i+15];

  checksum = (buf[i+30]<<8)|buf[i+31];  
  calchecksum = 0;  
  int k;
  for (k = 0; k < 30; k++)
    calchecksum += buf[i+k];
/******************************************************************************************************/
  if(calchecksum == checksum){
  /*********************************Setting the values of each channel*********************************/
    datastreams[0].setString(device_name);
    Serial.println(datastreams[0].getString());
    
    datastreams[1].setInt(pm10);
    Serial.print("The PM1.0 value: ");
    Serial.println(datastreams[1].getInt());
  
    datastreams[2].setInt(pm25);
    Serial.print("The PM2.5 value: ");
    Serial.println(datastreams[2].getInt());
  
    datastreams[3].setInt(pm100);
    Serial.print("The PM10 value: ");
    Serial.println(datastreams[3].getInt());
  /****************************************************************************************************/
  
    Serial.println("Uploading it to Xively");
    int ret = xivelyclient.put(feed, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
  }
  Serial.println();
  delay(15000);
}
