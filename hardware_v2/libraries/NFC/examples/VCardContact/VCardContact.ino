/*
 * This sketch demonstrates add contact on an andoird phone by 
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
 * phone add contact.
 * 
 */

#include <NfcTag.h>

#define MAX_VCARD_LEN 110
char vcard_buf[MAX_VCARD_LEN];

char name[] = "Forrest Gump";

char family_name[] = "Forrest";
char given_name[] = "Gump";
char additional_name[] = "";
char honorific_prefix[] = "";
char honorific_suffixes[] = "Mr.";

char phone[] = "(111) 555-1212";

void setup() {
  int vcard_len = 0;
  vcard_len = sprintf( vcard_buf, "BEGIN:VCARD\r\nVERSION:1.0\r\nN:%s\r\nFN:%s;%s;%s;%s;%s\r\nTEL;VOICE:%s\r\nEND:VCARD\r\n",
    name,
    family_name, given_name, additional_name, honorific_prefix, honorific_suffixes,
    phone
  );
  if (vcard_len <= MAX_VCARD_LEN) {
    NfcTag.appendVcard(vcard_buf, vcard_len);
    NfcTag.begin();
  } else {
    Serial.println("ERR: Invalid length of vCard!");
  }
}

void loop() {
  delay(1000);
}