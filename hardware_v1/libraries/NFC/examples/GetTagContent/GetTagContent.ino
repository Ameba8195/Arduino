/*
 * This sketch demonstrates get nfc content when tag content has been programmed
 * 
 * Pre-requirement:
 * (1) By default the NFC antenna is not connected. You have to weld it.
 * (2) The Android phone needs have NFC function.
 * 
 * Use Android NFC application which support nfc write function.
 * Try to write something on it, then Ameba will dump the message on log.
 */

#include <NfcTag.h>

uint32_t last_update_time;
const struct NDEF *pNdefMsg;

void setup() {
  NfcTag.appendRtdText("0");
  NfcTag.begin();

  last_update_time = NfcTag.getLastUpdateTimestamp();
}

void loop() {
  if (NfcTag.getLastUpdateTimestamp() > last_update_time)
  {
    last_update_time = NfcTag.getLastUpdateTimestamp() ;
    dumpTagContent();
  }
  delay(1000);
}

void dumpTagContent() {
  pNdefMsg = NfcTag.getNdefData();
  for (int i=0; i<NfcTag.getNdefSize(); i++)
  {
    // print the ndef data
    Serial.print("NDEF message ");
    Serial.println(i);

    Serial.print("\tMB: ");
    Serial.print( (pNdefMsg[i].TNF_flag & TNF_MESSAGE_BEGIN) ? 1 : 0);
    Serial.print("\tME: ");
    Serial.print( (pNdefMsg[i].TNF_flag & TNF_MESSAGE_END) ? 1 : 0);
    Serial.print("\tCF: ");
    Serial.print( (pNdefMsg[i].TNF_flag & TNF_MESSAGE_CHUNK_FLAG) ? 1 : 0);
    Serial.print("\tSR: ");
    Serial.print( (pNdefMsg[i].TNF_flag & TNF_MESSAGE_SHORT_RECORD) ? 1 : 0);
    Serial.print("\tIL: ");
    Serial.print( (pNdefMsg[i].TNF_flag & TNF_MESSAGE_ID_LENGTH_IS_PRESENT) ? 1 : 0);
    Serial.println();

    Serial.print("\tTNF Type: ");
    switch( pNdefMsg[i].TNF_flag & 0x07 ) {
      case TNF_EMPTY:         Serial.println("EMPTY");         break;
      case TNF_WELL_KNOWN:    Serial.println("WELL_KNOWN");    break;
      case TNF_MIME_MEDIA:    Serial.println("MIME_MEDIA");    break;
      case TNF_ABSOLUTE_URI:  Serial.println("ABSOLUTE_URI");  break;
      case TNF_EXTERNAL_TYPE: Serial.println("EXTERNAL_TYPE"); break;
      case TNF_UNCHANGED:     Serial.println("UNCHANGED");     break;
      case TNF_RESERVED:      Serial.println("RESERVED");      break;
      default:                Serial.println();                break;
    }

    Serial.print("\ttype length: ");
    Serial.println(pNdefMsg[i].type_len);
    Serial.print("\ttype: ");
    Serial.println((char *)(pNdefMsg[i].payload_type));

    Serial.print("\tpayload length: ");
    Serial.println(pNdefMsg[i].payload_len);
    Serial.print("\tpayload: ");
    Serial.println((char *)(pNdefMsg[i].payload));
  }
}