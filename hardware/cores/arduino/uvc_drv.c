#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "uvc_drv.h"

#include <inttypes.h>
#include "FreeRTOS.h"
#include "task.h"

#include "rtsp/rtsp_api.h"
#include "uvc_intf.h"

#include "sockets.h"
#include "lwip/netif.h"

struct rtp_object rtp_payload;
struct uvc_context current_uvc_contex = {UVC_FORMAT_MJPEG, 640, 480, 30, 0};

void set_uvc_contex(int frame_type, int width, int height, int frame_rate, int compression_ratio) {
    current_uvc_contex.fmt_type = (uvc_fmt_t)frame_type;
    current_uvc_contex.width = width;
    current_uvc_contex.height = height;
    current_uvc_contex.frame_rate = frame_rate;
    current_uvc_contex.compression_ratio = compression_ratio;
}

int set_video(struct uvc_context *uvc_ctx, struct stream_flow *stream) {
    int codec_id;

    codec_id = format2codec_id(uvc_ctx->fmt_type);

    if(uvc_set_param(uvc_ctx->fmt_type, &uvc_ctx->width, &uvc_ctx->height, &uvc_ctx->frame_rate, &uvc_ctx->compression_ratio) < 0) {
        return -1;
    }

    if ( rtsp_set_src_FormatInfo(stream, codec_id, uvc_ctx->frame_rate, uvc_ctx->height, uvc_ctx->width, 0) < 0 ) {
        return -1;
    }

    return 0;
}

int queue_rtp2src(struct stream_context *stream_ctx, struct stream_flow *stream) {
    struct rtp_object *payload;
    payload = rtp_object_out_stream_queue(stream_ctx);
	if(payload == NULL)
	    return -1;
	rtp_object_in_src_queue(payload, stream);
	return 0;
}

int queue_src2rtp(struct stream_flow *stream, struct stream_context *stream_ctx) {
    struct rtp_object *payload;
	payload = rtp_object_out_src_queue(stream);
	if(payload == NULL)
	    return -1;
	rtp_object_in_stream_queue(payload, stream_ctx);
	return 0;
}

static int uvc_src_handler(void *src, struct rtp_object *payload) {

    struct uvcManager *manager = (struct uvcManager *) src;
    struct uvc_buf_context buf;
    int ret = 0;

    /*load rtp object handler here by codec id*/
    rtp_load_o_handler_by_codec_id( payload, manager->currentFormatInfo.codec.codec_id );
    if(payload->rtp_object_handler == NULL)
        return -1;

    /*queue back used uvc buffer for new data*/
    if( payload->state == RTP_OBJECT_USED ) {
        memset(&buf, 0, sizeof(struct uvc_buf_context));

        buf.index = payload->index;
        ret = uvc_qbuf(&buf);
        if (ret < 0){
            uvc_stream_off();
            return ret;
        }
    }

    /* get uvc buffer for new data */
    ret = uvc_dqbuf(&buf);
    if(buf.index < 0) {
        // empty buffer retrieved set payload state as IDLE
        payload->state = RTP_OBJECT_IDLE;
        return 0;
    }

    if( (uvc_buf_check(&buf) < 0) || (ret < 0) ) {
        uvc_stream_off();
        return ret;
    }

    /*manipulate buf data if necessary*/
    //nothing to do...

    /*fill payload*/
    payload->index = buf.index;
    payload->data = buf.data;
    payload->len = buf.len;

    /* because we will fill & send a complete frame in single rtp object, set both fs & fe to 1 and fd to 0 */
    rtp_object_set_fs(payload, 1);
    rtp_object_set_fe(payload, 1);
    rtp_object_set_fd(payload, 0);

    /* set payload state to READY to enable rtp task process this rtp object;*/
    payload->state = RTP_OBJECT_READY;        

    return ret;
}

void uvc_task(void *param) {

    struct rtsp_context *rtsp_ctx = NULL;
    struct stream_flow *video_stream = NULL;
    struct uvcManager *manager = NULL;
    struct uvc_buf_context buf; 
    struct uvc_user_ctrl ctrl;
    int ret;

    while( !uvc_is_stream_ready() ) {
        vTaskDelay( 100 / portTICK_RATE_MS );
    }

    do {
        /* create an rtsp context that can allow 1 stream */
        rtsp_ctx = rtsp_context_create(1); 
        if( rtsp_ctx == NULL ) {
            break;
        }

        /* create a video stream source flow */
        video_stream = rtsp_stream_flow_create();
        if( video_stream == NULL ) {
            break;
        }

        manager = malloc(sizeof(struct uvcManager));
        if( manager == NULL ) {
            break;
        }

        /*open src2rtsp service task*/
        rtsp_stream_flow_set(video_stream, (void *)manager, SRC_TYPE_UVC);
        if( src2rtsp_open(video_stream) < 0 ) {
    	    break;
    	}

        /*open rtsp service task*/
        if( rtsp_open(rtsp_ctx) < 0 ) {
            break;
        }

        /*init & queue rtp object for use*/
        rtp_object_init(&rtp_payload);
        rtp_object_in_stream_queue(&rtp_payload, &rtsp_ctx->stream_ctx[0]);

        /* register src handler callback if any */
        rtsp_register_src_handler(video_stream, uvc_src_handler);

        /*set video format information*/
        set_video(&current_uvc_contex, video_stream);

        /* add video stream into rtsp context */
        if( rtsp_add_stream(rtsp_ctx, video_stream) < 0 ) {
            break;
        }

        /* if you ever want to reset stream format which is already added in rtsp context, calling rtp_sync_stream is a must before restart rtsp service*/
        if( rtsp_sync_stream(rtsp_ctx, video_stream) < 0 ) {
            break;
        }

        /*wait until rtsp service is enabled */
        while( !rtsp_is_service_enabled(rtsp_ctx) ) {
            vTaskDelay( 1000 / portTICK_RATE_MS );
        }

        /*start camera*/
        if(uvc_stream_on() < 0) {
            break;
        }

        /*start streaming process*/	
        rtsp_start(rtsp_ctx);
        src2rtsp_start(video_stream);

        while( rtsp_is_stream_enabled(rtsp_ctx) && uvc_is_stream_on() ) {
            queue_rtp2src(&rtsp_ctx->stream_ctx[0], video_stream);
		    queue_src2rtp(video_stream, &rtsp_ctx->stream_ctx[0]);
    		vTaskDelay(10);
        }
    } while(0);

    vTaskDelay(1000);
    if ( rtsp_ctx != NULL ) {
        rtsp_close(rtsp_ctx);
    	rtsp_context_free(rtsp_ctx);
        rtsp_ctx = NULL;
    }

    vTaskDelay(1000);
    if ( video_stream != NULL ) {
        src2rtsp_close(video_stream);
    	rtsp_stream_flow_free(video_stream);
        video_stream = NULL;
    }
    
    rtp_object_deinit(&rtp_payload);

    if (manager != NULL) {
        free(manager);
        manager = NULL;
    }

    uvc_stream_free();

    vTaskDelete(NULL);
}

int v4l2_external_task(void) {
    if(xTaskCreate(uvc_task, ((const char*)"uvc_task"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        return -1;
    }
    return 0;
}


#ifdef __cplusplus
}
#endif
