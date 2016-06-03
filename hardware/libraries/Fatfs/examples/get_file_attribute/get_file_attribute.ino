/*
 This sketch shows how to check whether a item is a directory or a file.
 */

#include "SdFatFs.h"

SdFatFs fs;

void setup() {

  char buf[512];
  char absolute_filename[512];
  char *p;

  fs.begin();

  fs.readDir(fs.getRootPath(), buf, sizeof(buf));

  p = buf;
  while (strlen(p) > 0) {
    printf("\"%s\" is ", p);

    sprintf(absolute_filename, "%s%s", fs.getRootPath(), p);
    if (fs.isDir(absolute_filename)) {
      printf("dir\r\n");
    }

    if (fs.isFile(absolute_filename)) {
      printf("file\r\n");
    }

    p += strlen(p) + 1;
  }

  fs.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}