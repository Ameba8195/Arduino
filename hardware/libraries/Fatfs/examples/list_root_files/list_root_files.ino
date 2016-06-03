/*
 This sketch shows how to list files under root directory.
 */

#include "SdFatFs.h"

SdFatFs fs;

void setup() {

  char buf[512];
  char *p;

  fs.begin();

  /* list root directory and put results in buf */
  memset(buf, 0, sizeof(buf));
  fs.readDir(fs.getRootPath(), buf, sizeof(buf));

  /* the filenames are separated with '\0', so we scan one by one */
  p = buf;
  while (strlen(p) > 0) {
    printf("%s\r\n", p);
    p += strlen(p) + 1;
  }

  fs.end();
}

void loop() {

}