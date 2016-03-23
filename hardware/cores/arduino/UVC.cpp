#include "UVC.h"

#ifdef __cplusplus
extern "C" {
#include "uvc_intf.h"
#include "uvc_drv.h"
}
#endif

void UVCClass::begin(int frame_type, int width, int height, int frame_rate, int compression_ratio) {

    this->frame_type        = frame_type;
    this->width             = width;
    this->height            = height;
    this->frame_rate        = frame_rate;
    this->compression_ratio = compression_ratio;
    
    set_uvc_contex(frame_type, width, height, frame_rate, compression_ratio);
    uvc_stream_init();
}

void UVCClass::begin() {
    begin(UVC_MJPEG, 320, 240, 30, 0);
}

void UVCClass::end() {
    uvc_stream_free();
}

int UVCClass::available() {
    return uvc_is_stream_ready();
}

int UVCClass::status() {
    return uvc_is_stream_on();
}

int UVCClass::turnOn() {
    return uvc_stream_on();
}

void UVCClass::turnOff() {
    uvc_stream_off();
}

UVCClass UVC;
