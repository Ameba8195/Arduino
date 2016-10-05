/*
  This sketch shows how to enable OTA service and proceed your task at the same time

  For how to use OTA, please refer sketch "ota_basic".

*/

#include <WiFi.h>
#include <OTA.h>

char ssid[] = "yourNetwork";     //  your network SSID (name)
char pass[] = "secretPassword";  // your network password

#define MY_VERSION_NUMBER 1
#define OTA_PORT 5000
#define RECOVER_PIN 18

void setup() {
  printf("This is version %d\r\n\r\n", MY_VERSION_NUMBER);

  os_thread_create(wifi_service_thread, NULL, OS_PRIORITY_REALTIME, 2048);
}

void loop() {
  delay(1000);
  printf("Current system tick: %d\r\n", millis());
}

void wifi_service_thread(const void *argument) {
  uint32_t ota_thread_id = 0;
  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      printf("WiFi is not connected. Try to connect to %s\r\n", ssid);
      while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        printf("Failed. Wait 1s and retry...\r\n");
        delay(1000);
      }
      printf("Connected to %s\r\n", ssid);

      if (ota_thread_id == 0) {
        ota_thread_id = os_thread_create(ota_thread, NULL, OS_PRIORITY_REALTIME, 2048);
      }
    }
    delay(1000); // check wifi status every 1s
  }

  // This line is not expected to be executed because we want to keep checking wifi status
  os_thread_terminate( os_thread_get_id() );
}

void ota_thread(const void *argument) {
  // These setting only needed at first time download from usb. And it doesn't needed at next OTA.
#if MY_VERSION_NUMBER == 1
  // This set the flash address that store the OTA image. Skip this setting would use default setting which is DEFAULT_OTA_ADDRESS
  OTA.setOtaAddress(DEFAULT_OTA_ADDRESS);

  // This set the recover pin. Boot device with pull up this pin (Eq. connect pin to 3.3V) would make device boot from version 1
  OTA.setRecoverPin(RECOVER_PIN);
#endif

  // Broadcast mDNS service at OTA_PORT that makes Arduino IDE find Ameba device
  OTA.beginArduinoMdnsService(OTA_PORT);

  // Listen at OTA_PORT and wait for client (Eq. Arduino IDE). Client would send OTA image and make a update.
  while( OTA.beginLocal(OTA_PORT) < 0 ) {
    printf("Retry OTA after 10s\r\n");
    delay(10000);
  }

  // This line is not expected to be executed because if OTA success it would reboot device immediatedly.
  os_thread_terminate( os_thread_get_id() );
}