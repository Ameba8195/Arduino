#ifndef _UVC_INTF_H_
#define _UVC_INTF_H_

#ifdef ARDUINO_SDK
#include <inttypes.h>
#endif

enum uvc_format_type{
      UVC_FORMAT_MJPEG = 1,
      UVC_FORMAT_H264 = 2,
      UVC_FORMAT_UNKNOWN = -1,
};

typedef enum uvc_format_type uvc_fmt_t;

struct uvc_context
{
      uvc_fmt_t fmt_type; //video format type
      int width;//video frame width
      int height;//video frame height
      int frame_rate;//video frame rate
      int compression_ratio;//compression format video compression ratio
};

#define USER_CTRL_SATURATION  1

struct uvc_user_ctrl
{
#ifdef ARDUINO_SDK
      uint32_t ctrl_id;
      int32_t ctrl_value;
#else
      u32 ctrl_id;
      s32 ctrl_value;
#endif
};

struct uvc_buf_context
{
      int index; //index of internal uvc buffer
      unsigned char *data; //address of uvc data
      int len; //length of uvc data
#ifdef ARDUINO_SDK
      uint32_t timestamp; //timestamp
#else
      u32 timestamp; //timestamp
#endif
};

int uvc_stream_init(void); //entry function to start uvc
void uvc_stream_free(void); // free streaming resources
int uvc_is_stream_ready(void); // return true if uvc device is initialized successfully
int uvc_is_stream_on(void); //return true if uvc device is streaming now
int uvc_stream_on(void); //enable camera streaming
void uvc_stream_off(void); //disable camera streaming
int uvc_set_param(uvc_fmt_t fmt_type, int *width, int *height, int *frame_rate, int *compression_ratio);//set camera streaming video parameters:video format, resolution and frame rate.
int uvc_get_user_ctrl(struct uvc_user_ctrl *user_ctrl);
int uvc_set_user_ctrl(struct uvc_user_ctrl *user_ctrl);
int uvc_buf_check(struct uvc_buf_context *b); //check if uvc_buf_context is legal (return 0 is legal otherwise -1)
int uvc_dqbuf(struct uvc_buf_context *b); //dequeue internal buffer & get internal buffer info
int uvc_qbuf(struct uvc_buf_context *b); //queue internal buffer
int is_pure_thru_on(void);  //return 1 if pure throughput test mode is on otherwise return 0
void uvc_pure_thru_on(void); //turn on pure uvc throughput test mode (i.e. no decoding is involved)
void uvc_dec_thru_on(void); //turn on uvc throughput test mode with uvc payload decoding
void uvc_thru_off(void);    //turn off uvc throughput log service

#endif
