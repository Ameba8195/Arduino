#if defined(BOARD_RTL8710)
#error "RTL8710 do not support UVC"
#endif

#include "Arduino.h"
#include "UVC.h"

#ifdef __cplusplus
extern "C" {

#include "cmsis_os.h"

#include "mmf_source.h"
#include "mmf_sink.h"

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

void uvcThread(void const *argument) {

    int i, ret;
    osEvent evt;

    UVCClass *pUVC = (UVCClass *)argument;

    pUVC->msink_ctx = NULL;
    pUVC->msrc_ctx = NULL;

    xQueueHandle src2sink_qid;
    xQueueHandle sink2src_qid;

    src2sink_qid = xQueueCreate(2, sizeof(exch_buf_t));
    sink2src_qid = xQueueCreate(2, sizeof(exch_buf_t));

    switch(pUVC->app_type) {
        case RTSP_STREAMING: {
            ret = 0;
            do {
                pUVC->msink_ctx = mmf_sink_open(&rtsp_module);
                if ( pUVC->msink_ctx == NULL ) {
                    ret = -1;
                    break;
                }

                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_FRAMERATE, pUVC->frame_rate);
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_BITRATE, 0);
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_CODEC, FMT_V_MJPG);
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_APPLY, 0);
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_INPUT_QUEUE, (int)src2sink_qid);
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_OUTPUT_QUEUE, (int)sink2src_qid);       
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_STREAMMING, 1);
                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_TASK_ON, 0);

                pUVC->msrc_ctx = mmf_source_open(&uvc_module);
                if ( pUVC->msrc_ctx == NULL ) {
                    ret = -2;
                    break;
                }

                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_FRAMETYPE, FMT_V_MJPG);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_HEIGHT, pUVC->height);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_WIDTH, pUVC->width);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_FRAMERATE, pUVC->frame_rate);   
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_CPZRATIO, pUVC->compression_ratio);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_APPLY, 0);
                
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_INPUT_QUEUE, (int)sink2src_qid);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_OUTPUT_QUEUE, (int)src2sink_qid);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_STREAMMING, 1);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_TASK_ON, 0);

                while (1) {
                    evt = osSignalWait(0, 0xFFFFFFFF);
                    if (evt.status == osEventSignal) {
                        if (evt.value.signals & REQ_END_STREAM) {
                            break;
                        }
                    }
                }

                mmf_sink_ctrl((msink_context *)(pUVC->msink_ctx), CMD_SET_STREAMMING, 0);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_STREAMMING, 0);

            } while (0);

            if (ret < 0) {
                printf("Fail to init UVC\r\n");
            }

            mmf_sink_close((msink_context *)(pUVC->msink_ctx));
            mmf_source_close((msrc_context *)(pUVC->msrc_ctx));

            break;
        }
        case JPEG_CAPTURE: {
            ret = 0;
            exch_buf_t exbuf;
            uint32_t exbuf_len_avg = 0;
            memset(&exbuf, 0, sizeof(exch_buf_t));

            do {
                pUVC->msrc_ctx = mmf_source_open(&uvc_module);
                if ( pUVC->msrc_ctx == NULL ) {
                    ret = -2;
                    break;
                }

                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_FRAMETYPE, FMT_V_MJPG);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_HEIGHT, pUVC->height);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_WIDTH, pUVC->width);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_FRAMERATE, pUVC->frame_rate);   
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_CPZRATIO, pUVC->compression_ratio);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_APPLY, 0);
                
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_OUTPUT_QUEUE, (int)src2sink_qid);
                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_STREAMMING, 1);

                while (1) {
                    evt = osSignalWait(0, 0xFFFFFFFF);
                    if (evt.status == osEventSignal) {
                        if (evt.value.signals & REQ_END_STREAM) {
                            break;
                        } else if (evt.value.signals & REQ_JPEG_CAPTURE) {
                            for ( i=0; i<FILTER_FACTOR; i++) {
                                mmf_source_get_frame((msrc_context *)(pUVC->msrc_ctx), &exbuf);
                                if (exbuf.data != NULL && exbuf.len > 0) {
                                    exbuf_len_avg += exbuf.len;
                                }
                                exbuf.state = STAT_USED;
                            }
                            exbuf_len_avg /= FILTER_FACTOR;

                            while (pUVC->mjpeg_size == 0) {
                                mmf_source_get_frame((msrc_context *)(pUVC->msrc_ctx), &exbuf);

                                if(exbuf.data != NULL) {
                                    // check if current jpeg size fall between 60%~140% of previous 5 jpeg average
                                    if (exbuf.len > (exbuf_len_avg * 6 / 10) && exbuf.len < exbuf_len_avg * 14 / 10) {
                                        memcpy(pUVC->mjpeg_buf, exbuf.data, exbuf.len);
                                        pUVC->mjpeg_size = exbuf.len;
                                    }
                                }

                                exbuf.state = STAT_USED;
                            }
                        }
                    }
                }

                mmf_source_ctrl((msrc_context *)(pUVC->msrc_ctx), CMD_SET_STREAMMING, 0);

            } while (0);

            if (ret < 0) {
                printf("Fail to init UVC\r\n");
            }

            mmf_source_close((msrc_context *)(pUVC->msrc_ctx));

            break;
        }
    }

    osThreadTerminate(pUVC->uvctid);
}

UVCClass::UVCClass() {
    uvctid = NULL;
    mjpeg_buf = NULL;
    mjpeg_size = 0;

    msink_ctx = NULL;
    msrc_ctx = NULL;
}

UVCClass::~UVCClass() {
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
    int avail = 0;
    if (msrc_ctx != NULL) {
        mmf_source_ctrl((msrc_context *)(msrc_ctx), CMD_GET_STREAM_READY, (int)(&avail));
    }
    return avail;
}

int UVCClass::status() {
    int avail = 0;
    if (msrc_ctx != NULL) {
        mmf_source_ctrl((msrc_context *)(msrc_ctx), CMD_GET_STREAM_STATUS, (int)(&avail));
    }
    return avail;
}

int UVCClass::turnOn() {
    mmf_sink_ctrl((msink_context *)(msink_ctx), CMD_SET_STREAMMING, 1);
    mmf_source_ctrl((msrc_context *)(msrc_ctx), CMD_SET_STREAMMING, 1);
}

void UVCClass::turnOff() {
    mmf_sink_ctrl((msink_context *)(msink_ctx), CMD_SET_STREAMMING, 0);
    mmf_source_ctrl((msrc_context *)(msrc_ctx), CMD_SET_STREAMMING, 0);
}

int UVCClass::getJPEG(unsigned char* buf) {
    int ret = 0;
    int retry;

    mjpeg_buf = buf;
    mjpeg_size = 0;

    osSignalSet(uvctid, REQ_JPEG_CAPTURE);

    for (retry = 0; retry<10; retry++) {
        delay(50);
        if (mjpeg_size > 0) {
            break;
        }
    }

    ret = mjpeg_size;
    mjpeg_size = 0;

    return ret;
}

UVCClass UVC;
