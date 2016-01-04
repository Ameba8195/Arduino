#include <NfcTag.h>

void setup() {
  NfcTag.appendWellKnownText("HELLO WORLD!");
  NfcTag.begin();
}

void loop() {
  delay(1000);
}
