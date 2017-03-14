/*
 * Demonstrates use of deepsleep api of power management with DHTxx and lass
 *
 * This sketch gather DHTxx results for every one minute.
 * After gather several data sets, publish them to LASS.
 * Ameba would under deepsleep between 2 measurements for saving power.
 *
 * You can find more information of LASS at this site:
 *     https://lass.hackpad.com/LASS-README-DtZ5T6DXLbu
 *
 * If you accidentally put deepsleep at the beginning,
 * you can connect D18(For RTL8710, it's D2) to GND. It's a software lock of power save
 * which prevent Ameba enter any power save state.
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include "FlashMemory.h"
#include <PowerManagement.h>r
#include "DHT.h"

char ssid[] = "yourNetwork";    // your network SSID (name)
char pass[] = "secretPassword"; // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)

char gps_lat[] = "24.7805647";  // device's gps latitude
char gps_lon[] = "120.9933177"; // device's gps longitude

int measureInterval = 600;  // in seconds

// gether 1 data then upload
#define DATA_CNT_FOR_UPLOAD 1
struct dhtData_t {
  char pattern[8];
  int dataCount;
  float h[DATA_CNT_FOR_UPLOAD];
  float t[DATA_CNT_FOR_UPLOAD];
};
struct dhtData_t *pDhtData;

#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

char server[] = "gpssensor.ddns.net"; // the MQTT server of LASS
char clientId[17] = "";                    // client id for MQTT
char outTopic[20] = "LASS/Test/PM25/live"; // MQTT publish topic

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

WiFiUDP Udp;
const char ntpServer[] = "pool.ntp.org";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
const byte nptSendPacket[ NTP_PACKET_SIZE] = {
  0xE3, 0x00, 0x06, 0xEC, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x31, 0x4E, 0x31, 0x34,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
byte ntpRecvBuffer[ NTP_PACKET_SIZE ];

#define LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )
static  const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0
uint32_t epochSystem = 0; // timestamp of system boot up

void reconnectWiFi() {
  // attempt to connect to Wifi network:
  if ( WiFi.status() != WL_CONNECTED ) {
    Serial.print("Try connect to wifi ssid: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      delay(1000);
    }
    Serial.println("Connected to wifi");

    Udp.begin(2390); // local port to listen for UDP packets
  }
}

// send an NTP request to the time server at the given address
void retrieveNtpTime() {
  for (int retry = 0; retry < 5; retry ++) {
    Serial.println("Send NTP packet");
    Udp.beginPacket(ntpServer, 123); //NTP requests are to port 123
    Udp.write(nptSendPacket, NTP_PACKET_SIZE);
    Udp.endPacket();

    delay(3000);
    if (Udp.parsePacket()) {
      Serial.println("NTP packet received");
      Udp.read(ntpRecvBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

      unsigned long highWord = word(ntpRecvBuffer[40], ntpRecvBuffer[41]);
      unsigned long lowWord = word(ntpRecvBuffer[42], ntpRecvBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;

      epochSystem = epoch - millis() / 1000;
      break;
    }
  }
}

void getCurrentTime(unsigned long epoch, int *year, int *month, int *day, int *hour, int *minute, int *second) {
  int tempDay = 0;

  *hour = (epoch  % 86400L) / 3600;
  *minute = (epoch  % 3600) / 60;
  *second = epoch % 60;

  *year = 1970; // epoch starts from 1970
  *month = 0;
  *day = epoch / 86400;

  for (*year = 1970; ; (*year)++) {
    tempDay += (LEAP_YEAR(*year) ? 366 : 365);
    if (tempDay > *day) {
      tempDay -= (LEAP_YEAR(*year) ? 366 : 365);
      break;
    }
  }
  tempDay = *day - tempDay; // the days left in a year
  for ((*month) = 0; (*month) < 12; (*month)++) {
    if ((*month) == 1) {
      tempDay -= (LEAP_YEAR(*year) ? 29 : 28);
      if (tempDay < 0) {
        tempDay += (LEAP_YEAR(*year) ? 29 : 28);
        break;
      }
    } else {
      tempDay -= monthDays[(*month)];
      if (tempDay < 0) {
        tempDay += monthDays[(*month)];
        break;
      }
    }
  }
  *day = tempDay + 1; // one for base 1, one for current day
  (*month)++;
}

bool sendMQTT() {
  char payload[300];
  unsigned long epoch = epochSystem + millis() / 1000;
  int year, month, day, hour, minute, second;

  if (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection");
    mqttClient.connect(clientId);
  }

  if (mqttClient.connected()) {
    for (int i=0; i<DATA_CNT_FOR_UPLOAD; i++) {
      getCurrentTime(epoch - (DATA_CNT_FOR_UPLOAD - i - 1) * measureInterval,
        &year, &month, &day, &hour, &minute, &second);
      sprintf(payload, "|ver_format=3|FAKE_GPS=1|app=PM25|ver_app=%s|device_id=%s|date=%4d-%02d-%02d|time=%02d:%02d:%02d|s_t0=%3.1f|s_h0=%2.1f|gps_lon=%s|gps_lat=%s",
              "live", clientId, year, month, day, hour, minute, second, pDhtData->t[i], pDhtData->h[i], gps_lon, gps_lat);
      mqttClient.publish(outTopic, payload);
    }
    return true;
  } else {
    return false;
  }
}

void initializeMQTT() {
  byte mac[6];

  WiFi.macAddress(mac);
  sprintf(clientId, "FT_LIVE_%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);

  Serial.print("MQTT client id:");
  Serial.println(clientId);
  Serial.print("MQTT topic:");
  Serial.println(outTopic);

  mqttClient.setServer(server, 1883);
}

int gatherHumidityAndTemperature() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  while (isnan(h) || isnan(t)) {
    Serial.println("DHTxx retry");
    delay(2000);
    h = dht.readHumidity();
    t = dht.readTemperature();
  }

  // read previous data from flash memory
  FlashMemory.read();
  pDhtData = (struct dhtData_t *)(FlashMemory.buf);

  if (strncmp(pDhtData->pattern, "AmebaDht", 8) != 0) {
    // the flash memory contain invalid data
    Serial.println("Invalid pattern");
    strncpy(pDhtData->pattern, "AmebaDht", 8);
    pDhtData->dataCount = 0;
  }
  if (pDhtData->dataCount >= DATA_CNT_FOR_UPLOAD) {
    // The data count is un-expected. Reset to 0.
    pDhtData->dataCount = 0;
  }
  pDhtData->h[pDhtData->dataCount] = h;
  pDhtData->t[pDhtData->dataCount] = t;
  pDhtData->dataCount = pDhtData->dataCount + 1;

  Serial.print("data count: ");
  Serial.println(pDhtData->dataCount);

  return pDhtData->dataCount;
}

void setup()
{
  dht.begin();

  if (gatherHumidityAndTemperature() == DATA_CNT_FOR_UPLOAD) {
    reconnectWiFi();
    retrieveNtpTime();

    initializeMQTT();
    while(!sendMQTT()) {
      delay(1000);
    }
    mqttClient.loop();

    pDhtData->dataCount = 0;
  }

  // store data back to flash memory
  FlashMemory.update();

  mqttClient.disconnect();

  Serial.println("deepsleep");
  PowerManagement.deepsleep(measureInterval * 1000);
}

void loop()
{
  delay(1000);
}