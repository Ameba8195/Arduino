/*
 * This sketch demonstrates open web page on an andoird phone by 
 * tapping nfc antenna on Ameba.
 * 
 * Pre-requirement:
 * (1) By default the NFC antenna is not connected. You have to weld it.
 * (2) The Android phone needs have NFC function.
 * 
 * Android framework support NDEF (NFC Data Exchange Format) data message.
 * As we configure NDEF message on Ameba's NFC firmware, tap phone on it, 
 * then Android framework will parse the NDEF messages and performs 
 * correspond actions.
 * 
 * This sketch demonstrates how to configure NDEF message that make Android 
 * phone open web page "http://www.amebaiot.com" on browser.
 * 
 */

#include <NfcTag.h>

void setup() {
  NfcTag.appendRtdUri("amebaiot.com");
  NfcTag.begin();
}

void loop() {
  delay(1000);
}