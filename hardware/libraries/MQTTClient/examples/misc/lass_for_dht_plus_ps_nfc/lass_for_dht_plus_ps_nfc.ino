#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <NfcTag.h>
#include <PowerManagement.h>
#include <FlashMemory.h>
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>

/* data for wifi connection */
char ssid[] = "yourNetwork";     //  your network SSID (name)
char pass[] = "secretPassword";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

/* data for MQTT*/
char server[]         = "gpssensor.ddns.net"; // the MQTT server of LASS

// position of Realtek
//char gps_lat[]        = "24.465003";  // device's gps latitude
//char gps_lon[]        = "120.593594"; // device's gps longitude

// position of National Taiwan Science Education Center
char gps_lat[]        = "25.05740";  // device's gps latitude
char gps_lon[]        = " 121.31000"; // device's gps longitude

char clientId[]       = "Ameba";
char topic[]          = "LASS/Test/PM25";
char payload[512];

WiFiClient wifiClient;
PubSubClient client(wifiClient);

/* data for flash memory */
#define DATA_SIGNATURE "12345678"
struct _memdata {
  char signature[8];
  float h;
  float t;
  time_t timestamp;
};
struct _memdata *memdata;

/* sensor data for DHT */
#define DHTPIN 16
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* data for NFC */
#define NFC_TEXT_SIZE 10
char nfctext[NFC_TEXT_SIZE];

/* data for time */
const int timeZone = 8;     // Beijing Time, Taipei Time

/* data for power saving */
//int measurement_interval = 10 * 60000; // 30s
int measurement_interval = 10000;

#define DBG (1)

void setup() {
  int retrytime;

  setSyncProvider(RTC.get);

  FlashMemory.read();
  memdata = (struct _memdata *) FlashMemory.buf;
  if ( strncmp(memdata->signature, DATA_SIGNATURE, 8) != 0 ) {
    // the content in flash is invalid, initialize it
    strncpy(memdata->signature, DATA_SIGNATURE, 8);
    memdata->h = 20;
    memdata->t = 20;
    memdata->timestamp = 1461728234UL;
  }

  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  time_t currentTime;
  if ( timeStatus() == timeSet) {
    currentTime = now();
    if (currentTime > 1461728234UL) {
      // the current time should larger than 2016/4/27
      memdata->timestamp = currentTime - timeZone * SECS_PER_HOUR;
    }
  }

  if ( !isnan(h) && !isnan(t) ) {
    // we got a valid data, store it.
    memdata->h = h;
    memdata->t = t;
  }
  memset(nfctext, 0, sizeof(nfctext));
  sprintf(nfctext, "h:%.0f,t:%.0f", memdata->h, memdata->t);

  NfcTag.appendRtdText(nfctext);
  NfcTag.begin();

  // store current content back to flash
  FlashMemory.update();

  /* prepare LASS data */
  memset(payload, 0, sizeof(payload));
  sprintf(payload, "|ver_format=3|fmt_opt=0|app=PM25|ver_app=0.0.1|device_id=%s|tick=%d|date=%4d-%02d-%02d|time=%02d:%02d:%02d|device=Ameba|s_h0=%.1f|s_t0=%.1f|gps_lat=%s|gps_lon=%s|gps_fix=1|gps_num=9|gps_alt=2",
    clientId,
    millis(),
    year(memdata->timestamp), month(memdata->timestamp), day(memdata->timestamp),
    hour(memdata->timestamp), minute(memdata->timestamp), second(memdata->timestamp),
    memdata->h, memdata->t,
    gps_lat, gps_lon
  );
  if (DBG) printf("%s\r\n", payload);

  // now try connecting to WiFi
  for (retrytime = 0; retrytime < 30; retrytime++) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) break;
    delay(500); // delay 0.5s for retry
  }

  if (status == WL_CONNECTED) {
    // setup MQTT
    client.setServer(server, 1883);
    for (retrytime; retrytime<30; retrytime++) {
      if (client.connect(clientId)) {
        client.publish(topic, payload);
        delay(1000);
        break;
      } else {
        Serial.println("LASS retry in 500ms");
        delay(500);
      }
    }
  }

  if(!PowerManagement.safeLock()) {
    if (DBG) printf("deepsleep\r\n");
    PowerManagement.deepsleep(measurement_interval);
  } else {
    if (DBG) printf("delay and softreset\r\n");
    delay(measurement_interval);
    PowerManagement.softReset();
  }
}

void loop() {
  delay(1000);
}