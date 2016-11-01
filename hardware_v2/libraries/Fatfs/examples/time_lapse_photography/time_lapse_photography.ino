/*
 This sketch shows how to capture jpeg from UVC campera and store it to sdcard
 */

#include <SdFatFs.h>
#include <UVC.h>

SdFatFs fs;

#define CAPTURE_INTERVAL (3 * 1000)

unsigned char jpegbuf[20 * 1024];

void setup() {

  UVC.begin(UVC_MJPEG, 640, 480, 30, 0, JPEG_CAPTURE);

  // wait until UVC is ready for streaming
  while (!UVC.available()) {
    delay(100);
  }
  delay(1000);
  Serial.println("UVC is ready");

  fs.begin();

}

int file_index = 0;
void loop() {
  char filename[128];
  int status;

  int len = UVC.getJPEG(jpegbuf);
  if (len > 0) {
    if (fs.status()) {

      sprintf(filename, "%s%04d.jpeg", fs.getRootPath(), file_index);
      printf("capture at \"%s\"\r\n", filename);

      SdFatFile file = fs.open(filename);
      file.write(jpegbuf, len);
      file.close();

      file_index++;

    } else {
      printf("fail to detect sdcard!\r\n");
    }
  }
  delay(CAPTURE_INTERVAL);
}