/*
 * This sketch demonstrates store nfc content to flash and restore it 
 * when device bootup
 * 
 * Pre-requirement:
 * (1) By default the NFC antenna is not connected. You have to weld it.
 * (2) The Android phone needs have NFC function.
 * 
 * Use Android NFC application which support nfc write function.
 * Try to write something on it and reboot Ameba to check if the content still exist.
 * 
 */

#include <NfcTag.h>
#include <FlashMemory.h>

int previousUpdateTimestamp;

bool isValidUid() {
  // UID has 2 byte checksum, and use it to check if flash content is valid
  if (FlashMemory.buf[3] != (0x88 ^ FlashMemory.buf[0] ^ FlashMemory.buf[1] ^ FlashMemory.buf[2])) {
    return false;
  }
  if (FlashMemory.buf[8] != (FlashMemory.buf[4] ^ FlashMemory.buf[5] ^ FlashMemory.buf[6] ^ FlashMemory.buf[7])) {
    return false;
  }
  return true;
}

void setup() {
  FlashMemory.read();
  if (isValidUid()) {
    Serial.println("Valid flash content and load to nfc");
    memcpy(NfcTag.nfc_tag_content, FlashMemory.buf, NFC_MAX_PAGE_NUM*4);
  } else {
    Serial.println("Invalid flash content, init as Hello World!");
    NfcTag.appendRtdText("Hello World!");
  }
  NfcTag.begin();
  previousUpdateTimestamp = NfcTag.getLastUpdateTimestamp();
}

void loop() {
  if (NfcTag.getLastUpdateTimestamp() > previousUpdateTimestamp) {
    Serial.println("nfc content has been update, store content to flash");
    previousUpdateTimestamp = NfcTag.getLastUpdateTimestamp();
    memcpy(FlashMemory.buf, NfcTag.nfc_tag_content, NFC_MAX_PAGE_NUM*4);
    FlashMemory.update();
  }
  delay(1000);
}