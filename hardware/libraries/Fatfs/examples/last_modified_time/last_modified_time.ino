/*
 This sketch shows how to get and set last modified time of a file.
 */

#include <SdFatFs.h>

SdFatFs fs;

char filename[] = "test.txt";

void setup() {
  char absolute_filename[128];

  uint16_t year   = 2016;
  uint16_t month  = 6;
  uint16_t date   = 3;
  uint16_t hour   = 16;
  uint16_t minute = 53;
  uint16_t second = 0;

  fs.begin();

  sprintf(absolute_filename, "%s%s", fs.getRootPath(), filename);
  SdFatFile file = fs.open(absolute_filename);
  file.close();

  fs.setLastModTime(absolute_filename, year, month, date, hour, minute, second);

  fs.getLastModTime(absolute_filename, &year, &month, &date, &hour, &minute, &second);
  printf("filename:\"%s\"\r\n", absolute_filename);
  printf("time mod:%04d/%02d/%02d %02d:%02d:%02d\r\n", year, month, date, hour, minute, second);

  fs.end();
}

void loop() {
}