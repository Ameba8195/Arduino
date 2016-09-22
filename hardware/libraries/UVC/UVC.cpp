#include "Arduino.h"
#include "UVC.h"

#ifdef __cplusplus
extern "C" {

#include "cmsis_os.h"

#include "usb.h"
#include "uvc_intf.h"
#include "rtsp/rtsp_api.h"

extern int format2codec_id(uvc_fmt_t fmt_type);

}
#endif

#define ARDUINO_UVC_DEBUG (0)

#ifdef UVC_PRINTF
#undef UVC_PRINTF
#endif

#if     ARDUINO_UVC_DEBUG
#define UVC_PRINTF(fmt, args...)	rtl_printf("%s: " fmt, __FUNCTION__, ## args)
#else
#define UVC_PRINTF(fmt, args...)
#endif


#define REQ_END_STREAM   0x01
#define REQ_JPEG_CAPTURE 0x02
#define FILTER_FACTOR	5

int set_video(struct uvc_context *uvc_ctx, struct stream_flow *stream)
{
    int codec_id = format2codec_id(uvc_ctx->fmt_type);

    if(uvc_set_param(uvc_ctx->fmt_type, &uvc_ctx->width, &uvc_ctx->height, &uvc_ctx->frame_rate, &uvc_ctx->compression_ratio) < 0) {
        return -1;
    }

    rtsp_set_src_FormatInfo(stream, codec_id, uvc_ctx->frame_rate, uvc_ctx->height, uvc_ctx->width, 0);

    return 0;
}

int queue_rtp2src(struct stream_context *stream_ctx, struct stream_flow *stream)
{
    struct rtp_object *payload;

    payload = rtp_object_out_stream_queue(stream_ctx);

	if(payload == NULL) {
	    return -1;
    }
	rtp_object_in_src_queue(payload, stream);

	return 0;
}

int queue_src2rtp(struct stream_flow *stream, struct stream_context *stream_ctx)
{
    struct rtp_object *payload;

	payload = rtp_object_out_src_queue(stream);

	if(payload == NULL) {
	    return -1;
    }
	rtp_object_in_stream_queue(payload, stream_ctx);

	return 0;
}

int uvc_src_handler(void *src, struct rtp_object *payload)
{
    struct uvcManager *manager = (struct uvcManager *) src;
    struct uvc_buf_context buf;
    int ret = 0;

    /*load rtp object handler here by codec id*/
    rtp_load_o_handler_by_codec_id(payload, manager->currentFormatInfo.codec.codec_id);
    if(payload->rtp_object_handler == NULL) {
        return -1;
    }

    /*queue back used uvc buffer for new data*/
    if(payload->state == RTP_OBJECT_USED)
    {
        memset(&buf, 0, sizeof(struct uvc_buf_context));
        buf.index = payload->index;
        //buf.data = payload->data;
        //buf.len = payload->len;
        ret = uvc_qbuf(&buf);                   
        if (ret < 0){
            UVC_PRINTF("read_frame mmap method enqueue buffer failed\r\n");
            uvc_stream_off();
            return ret;
        }
    }

    /*get uvc buffer for new data*/
    ret = uvc_dqbuf(&buf);                              
    if(buf.index < 0)
    {
        payload->state = RTP_OBJECT_IDLE;
        return 0;
    }//empty buffer retrieved set payload state as IDLE

    if( (uvc_buf_check(&buf) < 0) || (ret < 0) ){
        UVC_PRINTF("buffer error!\r\n");
        uvc_stream_off();
        return ret;
    }

    /*manipulate buf data if necessary*/

    /*fill payload*/
    payload->index = buf.index;
    payload->data = buf.data;
    payload->len = buf.len;
    //payload->timestamp = buf.timestamp;

    /* because we will fill&send a complete frame in single rtp object, set both fs & fe to 1 and fd to 0*/
    rtp_object_set_fs(payload, 1);
    rtp_object_set_fe(payload, 1);
    rtp_object_set_fd(payload, 0);

    /* set payload state to READY to enable rtp task process this rtp object;*/
    payload->state = RTP_OBJECT_READY;

    return ret;
}

void uvcThread(void const *argument) {

    int i, ret;

    UVCClass *pUVC = (UVCClass *)argument;

    uint8_t is_usb_init = 0;
    uint8_t is_uvc_stream_init = 0;
    uint8_t is_rtp_payload_init = 0;

    struct rtsp_context *rtsp_ctx = NULL;
    struct stream_flow *video_stream = NULL;
    struct uvcManager *manager = NULL;
    struct uvc_context uvc_ctx;
    struct uvc_buf_context buf;

    osEvent evt;

    switch(pUVC->app_type) {
        case RTSP_STREAMING:
        {
            do {
                is_usb_init = 1;
                _usb_init();
                if (wait_usb_ready() < 0) {
                    UVC_PRINTF("fail to init usb driver\r\n");
                    break;
                }
            
                if (uvc_stream_init() < 0) {
                    UVC_PRINTF("fail to init uvc stream\r\n");
                    break;
                }
                is_uvc_stream_init = 1;
                rtsp_ctx = rtsp_context_create(1);
                if (rtsp_ctx == NULL) {
                    UVC_PRINTF("create rtsp_ctx failed!\r\n");
                    break;
                }
                
                video_stream = rtsp_stream_flow_create();
                if (video_stream == NULL) {
                    UVC_PRINTF("create stream flow failed!\r\n");
                    break;
                }
                manager = (struct uvcManager *) malloc( sizeof(struct uvcManager) );
                if (manager == NULL) {
                    UVC_PRINTF("create source manager failed!\r\n");
                    break;
                }
                
                rtsp_stream_flow_set(video_stream, (void *)manager, SRC_TYPE_UVC);
                if (src2rtsp_open(video_stream) < 0) {
                    UVC_PRINTF("src2rtsp_open fail!\r\n");
                    break;
                }
                
                if(rtsp_open(rtsp_ctx) < 0) {
                    UVC_PRINTF("rtsp_open fail!\r\n");
                    break;
                }
                
                /*init & queue rtp object for use*/
                rtp_object_init((rtp_object*)(pUVC->rtp_payload));
                is_rtp_payload_init = 1;
                
                rtp_object_in_stream_queue((rtp_object*)(pUVC->rtp_payload), &rtsp_ctx->stream_ctx[0]); 
                /* register src handler callback if any */
                rtsp_register_src_handler(video_stream, uvc_src_handler);

                /*set video format information*/
                uvc_ctx.fmt_type = (uvc_fmt_t)(pUVC->frame_type);
                uvc_ctx.width = pUVC->width;
                uvc_ctx.height = pUVC->height;
                uvc_ctx.frame_rate = pUVC->frame_rate;  

                set_video(&uvc_ctx, video_stream);

                uvc_set_param(uvc_ctx.fmt_type, &uvc_ctx.width, &uvc_ctx.height, &uvc_ctx.frame_rate, &uvc_ctx.compression_ratio);

                /* add video stream into rtsp context */
                if(rtsp_add_stream(rtsp_ctx, video_stream) < 0) {
                    UVC_PRINTF("rtsp_add_stream fail!\r\n");
                    break;
                }
                
                /* if you ever want to reset stream format which is already added in rtsp context, calling rtp_sync_stream is a must before restart rtsp service*/
                if(rtsp_sync_stream(rtsp_ctx, video_stream) < 0) {
                    UVC_PRINTF("rtsp_sync_stream fail!\r\n");
                    break;
                }
                
                /*wait until rtsp service is enabled */
                for (i=0; i<10; i++) {
                    if ( rtsp_is_service_enabled(rtsp_ctx) ) {
                        break;
                    }
                    delay( 1000 );
                }
                if (i==10) {
                    UVC_PRINTF("wait rtsp service time out...\r\n");
                    break;
                }

                /*start camera*/
                if(uvc_stream_on() < 0) {
                    UVC_PRINTF("uvc_stream_on fail!\r\n");
                    break;
                }

                /*start streaming process*/ 
                rtsp_start(rtsp_ctx);
                src2rtsp_start(video_stream);
                
                while( rtsp_is_stream_enabled(rtsp_ctx) && uvc_is_stream_on() ) {
                    queue_rtp2src(&rtsp_ctx->stream_ctx[0], video_stream);
                    queue_src2rtp(video_stream, &rtsp_ctx->stream_ctx[0]);
                
                    evt = osSignalWait(0, 1);
                    if (evt.status == osEventSignal) {
                        if (evt.value.signals & REQ_END_STREAM) {
                            break;
                        }
                    }
                }
            } while (0); // end of UVC_RTSP_STREAMING

            break;
        }
        case JPEG_CAPTURE:
        {
            do {
                is_usb_init = 1;
                _usb_init();
                if (wait_usb_ready() < 0) {
                    UVC_PRINTF("fail to init usb driver\r\n");
                    break;
                }
        
                if (uvc_stream_init() < 0) {
                    UVC_PRINTF("fail to init uvc stream\r\n");
                    break;
                }
                is_uvc_stream_init = 1;
        
                /*set video format information*/
                uvc_ctx.fmt_type = (uvc_fmt_t)(pUVC->frame_type);
                uvc_ctx.width = pUVC->width;
                uvc_ctx.height = pUVC->height;
                uvc_ctx.frame_rate = pUVC->frame_rate;  
                uvc_ctx.compression_ratio = pUVC->compression_ratio; 
        
                uvc_set_param(uvc_ctx.fmt_type, &uvc_ctx.width, &uvc_ctx.height, &uvc_ctx.frame_rate, &uvc_ctx.compression_ratio);
        
        
                /*start camera*/
                if(uvc_stream_on() < 0) {
                    UVC_PRINTF("uvc_stream_on fail!\r\n");
                    break;
                }

                do {
                    evt = osSignalWait(0, 0xFFFFFFFF);
                    if (evt.status == osEventSignal) {
                        ret = uvc_dqbuf(&buf);

                        if(buf.index < 0) {
                            continue;
                        }

                        if( (uvc_buf_check(&buf) < 0) || (ret < 0) ){
                            UVC_PRINTF("buffer error\r\n");
                        }

                        if (evt.value.signals & REQ_JPEG_CAPTURE) {
                            if (pUVC->mjpeg_buf != NULL && pUVC->mjpeg_size == 0) {
                                memcpy(pUVC->mjpeg_buf, buf.data, buf.len);
                                pUVC->mjpeg_size = buf.len;
                            }
                        }
                        if (uvc_qbuf(&buf) < 0) {
                            uvc_stream_free();
                            break;
                        }
                    }        
                } while(uvc_is_stream_on());
        
            } while (0); // end of UVC_MJPEG_CAPTURE

            break;
        }
    }

    uvc_stream_off();

    if (manager != NULL) {
        free(manager);
        manager = NULL;
    }

    if (is_rtp_payload_init == 1) {
        rtp_object_deinit((rtp_object*)(pUVC->rtp_payload));
    }

    if (video_stream != NULL) {
        src2rtsp_close(video_stream);
        rtsp_stream_flow_free(video_stream);
        video_stream = NULL;
    }

    if (rtsp_ctx != NULL) {
        rtsp_close(rtsp_ctx);
        rtsp_context_free(rtsp_ctx);
        rtsp_ctx = NULL;
    }

    if (is_uvc_stream_init == 1) {
        uvc_stream_free();
    }

    if (is_usb_init == 1) {
        _usb_deinit();
        is_usb_init = 0;
    }

    osThreadTerminate(pUVC->uvctid);
}

UVCClass::UVCClass() {
    uvctid = NULL;
    rtp_payload = malloc(sizeof(struct rtp_object));
    mjpeg_buf = NULL;
    mjpeg_size = 0;
}

UVCClass::~UVCClass() {
    if (rtp_payload != NULL) {
        free(rtp_payload);
        rtp_payload = NULL;
    }
    if (mjpeg_buf != NULL) {
        free(mjpeg_buf);
        mjpeg_buf = NULL;
    }
}

void UVCClass::begin(int frame_type, int width, int height, int frame_rate, int compression_ratio, int app_type) {

    this->frame_type        = frame_type;
    this->width             = width;
    this->height            = height;
    this->frame_rate        = frame_rate;
    this->compression_ratio = compression_ratio;
    this->app_type          = app_type;

    osThreadDef_t uvc_thread_def = {
        uvcThread,
        osPriorityRealtime,
        1,
        UVC_THREAD_STACK_SIZE,
        "uvcthread"
    };
    uvctid = osThreadCreate (&uvc_thread_def, this);
}

void UVCClass::begin() {
    begin(UVC_MJPEG, 320, 240, 30, 0, RTSP_STREAMING);
}

void UVCClass::end() {
    if (uvctid != NULL) {
        osSignalSet(uvctid, REQ_END_STREAM);
    }
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

int UVCClass::getJPEG(unsigned char* buf) {
    int ret = 0;
    int retry;

    mjpeg_buf = buf;
    mjpeg_size = 0;

		for(int i = 0; i < FILTER_FACTOR; i++)
		{
			osSignalSet(uvctid, REQ_JPEG_CAPTURE);
			
			for (retry = 0; retry<10; retry++) {
				delay(50);
				if (mjpeg_size > 0) {
					break;
				}
			}
	
			ret = mjpeg_size;
			mjpeg_size = 0;
		}
    return ret;
}

UVCClass UVC;
