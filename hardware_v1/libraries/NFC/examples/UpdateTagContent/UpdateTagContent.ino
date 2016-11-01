/*
 * This sketch demonstrates update NFC content dynamically
 * 
 * Pre-requirement:
 * (1) By default the NFC antenna is not connected. You have to weld it.
 * (2) The Android phone needs have NFC function.
 * 
 * Use Android NFC application which support nfc read function.
 * This sketch update add-one counter at every second.
 * Use Android NFC reader application to get this value.
 */

#include <NfcTag.h>

int counter = 1;
char mybuf[10];

void setup() {
  NfcTag.appendRtdText("0");
  NfcTag.begin();
}

void loop() {
  counter++;
  Serial.print("counter:");
  Serial.println(counter);

  sprintf(mybuf, "%d", counter);
  NfcTag.clearNdefMessage();
  NfcTag.appendRtdText(mybuf);
  NfcTag.convertNdefToRaw();
  NfcTag.updateRawToCache();
  delay(1000);
}