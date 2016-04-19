#ifndef _UVC_H_
#define _UVC_H_

#include <inttypes.h>

#define UVC_THREAD_STACK_SIZE 2048

#define UVC_MJPEG 1
#define UVC_H264  2

#define RTSP_STREAMING 1
#define JPEG_CAPTURE  2

class UVCClass
{
public:
    UVCClass();
    ~UVCClass();
    
    void begin(int frame_type, int width, int height, int frame_rate, int compression_ratio, int app_type=RTSP_STREAMING);
    void begin();
    void end();

    int available();
    int status();

    int  turnOn();
    void turnOff();

    int getJPEG(unsigned char* buf);

    friend void uvcThread(void const *argument);

public:
    uint8_t *mjpeg_buf;
    int mjpeg_size;

private:
    int frame_type;
    int width;
    int height;
    int frame_rate;
    int compression_ratio;
    int app_type;

    void *uvctid;
    void *rtp_payload;
};

extern UVCClass UVC;

#endif
