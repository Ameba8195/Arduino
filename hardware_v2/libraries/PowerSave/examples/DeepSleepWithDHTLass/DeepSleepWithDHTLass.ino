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

char ssid[] = "mynetwork";      // your network SSID (name)
char pass[] = "mypassword";     // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)

char gps_lat[] = "24.7805647";  // device's gps latitude
char gps_lon[] = "120.9933177"; // device's gps longitude

int measureInterval = 600;  // in seconds

// gether 10 data then upload
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

#define MAX_CLIENT_ID_LEN 10
#define MAX_TOPIC_LEN     50
char clientId[MAX_CLIENT_ID_LEN];
char outTopic[MAX_TOPIC_LEN];
char payload[300];

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

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
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
uint32_t epochSystem = 0; // timestamp of system boot up

// send an NTP request to the time server at the given address
void retrieveNtpTime() {
  Serial.println("Send NTP packet");

  Udp.beginPacket(ntpServer, 123); //NTP requests are to port 123
  Udp.write(nptSendPacket, NTP_PACKET_SIZE);
  Udp.endPacket();

  if(Udp.parsePacket()) {
    Serial.println("NTP packet received");
    Udp.read(ntpRecvBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    
    unsigned long highWord = word(ntpRecvBuffer[40], ntpRecvBuffer[41]);
    unsigned long lowWord = word(ntpRecvBuffer[42], ntpRecvBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;

    epochSystem = epoch - millis() / 1000;
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
  *day = tempDay+1; // one for base 1, one for current day
  (*month)++;
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void initializeWiFi() {
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    if (status == WL_CONNECTED) {
      break;
    }

    // wait 10 seconds for reconnection:
    delay(10000);
  }

  // local port to listen for UDP packets
  Udp.begin(2390);
}

void initializeMQTT() {
  byte mac[6];
  WiFi.macAddress(mac);
  memset(clientId, 0, MAX_CLIENT_ID_LEN);
  sprintf(clientId, "FT1_0%02X%02X", mac[4], mac[5]);
  sprintf(outTopic, "LASS/Test/Pm25Ameba/%s", clientId);

  Serial.print("MQTT client id:");
  Serial.println(clientId);
  Serial.print("MQTT topic:");
  Serial.println(outTopic);

  client.setServer(server, 1883);
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

void publishResults() {
  unsigned long epoch = epochSystem + millis() / 1000;
  int year, month, day, hour, minute, second;

  Serial.println("Upload results");
  for (int i=0; i<DATA_CNT_FOR_UPLOAD; i++) {
    // Assume each data is measured for every "measureInterval" seconds.
    getCurrentTime(epoch - (DATA_CNT_FOR_UPLOAD - i - 1) * measureInterval,
      &year, &month, &day, &hour, &minute, &second);

    sprintf(payload, "|ver_format=3|fmt_opt=1|app=Pm25Ameba|ver_app=0.0.1|device_id=%s|tick=%d|date=%4d-%02d-%02d|time=%02d:%02d:%02d|device=Ameba|h=%2.1f|t=%3.1f|gps_lat=%s|gps_lon=%s|gps_fix=1|gps_num=9|gps_alt=2",
      clientId,
      millis(),
      year, month, day,
      hour, minute, second,
      pDhtData->h[i],
      pDhtData->t[i],
      gps_lat, gps_lon
    );

    client.publish(outTopic, payload);
    delay(20); // make a small delay for hardware finish tasks
  }
}

void setup()
{
  dht.begin();

  if (gatherHumidityAndTemperature() == DATA_CNT_FOR_UPLOAD) {
    initializeWiFi();
    retrieveNtpTime();

    initializeMQTT();
    if (!client.connected()) {
      reconnectMQTT();
    }

    publishResults();
    pDhtData->dataCount = 0;
  }

  // store data back to flash memory
  FlashMemory.update();

  client.disconnect();

  Serial.println("deepsleep");
  PowerManagement.deepsleep(measureInterval * 1000);
}

void loop()
{
  delay(1000);
}