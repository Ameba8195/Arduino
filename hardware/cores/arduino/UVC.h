#ifndef _UVC_H_
#define _UVC_H_

#define UVC_MJPEG 1
#define UVC_H264  2

class UVCClass
{
public:
    void begin(int frame_type, int width, int height, int frame_rate, int compression_ratio);
    void begin();
    void end();

    int available();
    int status();

    int  turnOn();
    void turnOff();

private:
    int frame_type;
    int width;
    int height;
    int frame_rate;
    int compression_ratio;
};

extern UVCClass UVC;

#endif
