#ifndef _SRC2RTSP_H_
#define _SRC2RTSP_H_

/* Add by Ian -- This file contains parts used for rtsp to access lower layer stream src information*/

#include "dlist.h"
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"
#include "rtsp/rtp_api.h"
#include "avcodec.h"

#define SRC_FROM_UVC
//#define SRC_FROM_UAC
//#define SRC_FROM_SPI
#define SRC_FROM_STA

#define SRC_TYPE_NUL 0
#define SRC_TYPE_UVC 1
#define SRC_TYPE_SPI 2
#define SRC_TYPE_UAC 3
#define SRC_TYPE_STA 7

struct FormatInfo
{
    struct codec_info codec;
    u8 framerate;
    u32 bitrate;
    u32 height;
    u32 width;
};

struct stream_flow
{
        int id;
	void *prot_hook; //transport protocol hook
	void *src;
	int src_type;
	struct FormatInfo currentFormatInfo;
	int (*src_handler) (void *src, struct rtp_object *payload);
	struct list_head input_queue;
	_mutex input_lock;
	struct list_head output_queue;
	_mutex output_lock;
        u8 is_src2rtsp_init;
        u8 is_src2rtsp_start;
        _sema start_src2rtsp_sema;
	struct stream_flow *next;
};


#ifdef SRC_FROM_UVC 
#include "v4l2_intf.h"
struct uvc_streaming;
struct uvcManager
{
    struct uvc_streaming *stream;
    streaming_state state;
    struct FormatInfo currentFormatInfo;
};
#endif

#ifdef SRC_FROM_SPI

#endif

#ifdef SRC_FROM_UAC

#endif

#ifdef SRC_FROM_STA
struct rtsp_static_source_manager
{
    struct FormatInfo currentFormatInfo;	
};
#endif

struct stream_flow* rtsp_stream_flow_create(void);
void rtsp_stream_flow_free(struct stream_flow *stream);
void rtsp_register_src_handler(struct stream_flow *stream, int (*src_handler) (void *stream_src, struct rtp_object *payload));
int rtsp_get_src_currentFormatInfo(struct stream_flow *stream);
int rtsp_set_src_FormatInfo(struct stream_flow *stream, int codec_id, u8 framerate, u32 height, u32 width, u32 bitrate);
extern int rtsp_set_src_extra(struct stream_flow, void *param); 
int rtsp_stream_flow_set(struct stream_flow *stream, void *source, int src_type);
int rtsp_stream_flow_update(struct stream_flow *stream);
int rtsp_stream_flow_concat(struct stream_flow *stream, struct stream_flow *stream_next);
void rtp_object_in_src_queue(struct rtp_object *payload, struct stream_flow *stream);
struct rtp_object *rtp_object_out_src_queue(struct stream_flow *stream);


#endif