/*
 This sketch shows how to use LASS with power saving.
 The power source use solar panel and Li-Po battery.

 The precedure is as below:
 1. Setup watchdog:
    Sometimes the hardware or dirver may hit unexpected fault and invoke hardfault.
    In this situation only watchdog and reset system. We enable 2s watch dog and feed it in 1s GTimer
    handler. If the whole process exceend "measurement_timeout" then we just disable watchdog and
    enter deepsleep.
 2. Set time source:
	Use Time library and set time source as RTC
 3. Get previous measurement data from flash:
	Use flash data if we fail to get current data
 4. Get DHT data:
	Try to read humidity and temperature. Use previous data if failed.
 5. Setup NFC:
	Prepare NFC NDEF data. There are 2 NDEF records.
	(a) The first one is for android applicaton. This NDEF record will trigger event and lunch
        application accordingly.
    (b) The second one is text data with this format:
			device:FT1_9999,h:34.0,t:27.0
	When Android phone with NFC enabled it will lunch application and bring the text data to it.
    The sequence of 2 NDEF messages cannot exchange. Otherwise it won't lunch application.
 6. Get current time from RTC:
	Assume the time content in RTC has timezone calibrated. We need use UTC for LASS usage.
 7. Prepare LASS payload data
 8. Connect to WiFi
	Enbale Wifi cost signaficant power usage. So we enable WiFi in later part of whole process.
 9. Connect to LASS
 10.Deepsleep

*/

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <NfcTag.h>
#include <PowerManagement.h>
#include <FlashMemory.h>
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <GTimer.h>

/* whole timeout for all task */
int measurement_timeout = 30 * 1000; // 30s

/* data for wifi connection */
char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "secretPassword";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

/* data for MQTT*/
char server[]         = "gpssensor.ddns.net"; // the MQTT server of LASS

// position of Realtek
char *gps_location[2] = {"24.465003", "120.593594"}; // position of RTK (latitude & longitude)
//char *gps_location[2] = {"25.05740", "121.31000"}; // position of position of National Taiwan Science Education Center

char clientId[]       = "FT1_9999"; // Fill in your Field Try ID
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
#define NFC_TEXT_SIZE 64
char nfctext[NFC_TEXT_SIZE];

/* data for time */
const int timeZone = 8;     // Beijing Time, Taipei Time

/* data for power saving */
int measurement_interval = 10 * 60000; // 10min

#define DBG (1)

void deepsleepAndReset() {
  if(!PowerManagement.safeLock()) {
    if (DBG) printf("deepsleep\r\n");
    PowerManagement.deepsleep(measurement_interval);
  } else {
    if (DBG) printf("delay and softreset\r\n");
    delay(measurement_interval);
    PowerManagement.softReset();
  }
}

uint32_t startTime = 0;
void myTimerHandler(uint32_t data) {
  if (millis() - startTime < measurement_timeout) {
    wdt_reset();
  } else {
    // It takes too much time. Just go to deepsleep
    if (DBG) printf("timeout!\r\n");
    GTimer.stop(0);
    wdt_disable();
    deepsleepAndReset();
  }
}

void setup() {
  startTime = millis();

  // enable watchdog to avoid hardfault happen
  wdt_enable(2000);

  /* Use 1s periodical timer to feed watchdog. Hardfault handler has highest IRQ priority.
   * When hardfault happened it makes timer handler cannot feed watchdog and watchdog would restart the system. */
  GTimer.begin(0, 1 * 1000 * 1000, myTimerHandler);

  setSyncProvider(RTC.get);

  FlashMemory.read();
  memdata = (struct _memdata *) FlashMemory.buf;
  if ( strncmp(memdata->signature, DATA_SIGNATURE, 8) != 0 ) {
    // the content in flash is invalid, initialize it
    strncpy(memdata->signature, DATA_SIGNATURE, 8);
    memdata->h = 20;
    memdata->t = 20;
    memdata->timestamp = 0;
  }

  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if ( !isnan(h) && !isnan(t) ) {
    // we got a valid data, store it.
    memdata->h = h;
    memdata->t = t;
  }

  memset(nfctext, 0, sizeof(nfctext));
  sprintf(nfctext, "device:%s,h:%.1f,t:%.1f", clientId, memdata->h, memdata->t);
  NfcTag.appendAndroidPlayApp("com.realtek.sensortag");
  NfcTag.appendRtdText(nfctext);
  NfcTag.begin();

  time_t currentTime;
  for (int retrytime = 0; retrytime < 3; retrytime++) {
    if (timeStatus() == timeSet) {
      currentTime = now();
      if (DBG) printf("current time:%d\r\n", currentTime);
      // LASS use UTC time and need remove timezone difference
      memdata->timestamp = currentTime - timeZone * SECS_PER_HOUR;
      break;
    }
    delay(200);
  }

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
    gps_location[0], gps_location[1]
  );
  if (DBG) printf("%s\r\n", payload);

  // now try connecting to WiFi
  for (int retrytime = 0; retrytime < 3; retrytime++) {
    printf("Attempting to connect to WPA SSID: %s\r\n", ssid);
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) break;
    delay(500); // delay 0.5s for retry
  }
  if (status == WL_CONNECTED) {
    if (DBG) printf("Connected to %s\r\n", ssid);
  }

  if (status == WL_CONNECTED) {
    // setup MQTT
    client.setServer(server, 1883);
    for (int retrytime = 0; retrytime<3; retrytime++) {
      if (client.connect(clientId)) {
        if (DBG) printf("Connected to MQTT %s\r\n", server);
        client.publish(topic, payload);
        delay(500); // delay a little for waiting packet send out
        break;
      } else {
        Serial.println("LASS retry in 500ms");
        delay(500);
      }
    }
  }

  GTimer.stop(0);
  wdt_disable();
  deepsleepAndReset();
}

void loop() {
  delay(1000);
}