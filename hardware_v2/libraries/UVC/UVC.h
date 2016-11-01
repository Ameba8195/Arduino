/** @file UVC.h */

/** 
 * @defgroup UVC UVC
 * UVC API
 * @{
 */

#ifndef _UVC_H_
#define _UVC_H_

#if defined(BOARD_RTL8710)
#error "RTL8710 do not support UVC"
#endif

#include <inttypes.h>

#define UVC_THREAD_STACK_SIZE 2048

/**
 * @defgroup uvc_frame_type uvc_frame_type
 * The UVC frame type
 *
 * @{
 */
/** Motion JPEG */
#define UVC_MJPEG 1
/** H.264 */
#define UVC_H264  2
/** @} */ // end of uvc_frame_type

/**
 * @defgroup uvc_app_type uvc_app_type
 *
 * The usage of UVCClass.
 *
 * @{
 */
/* RTSP streaming */
#define RTSP_STREAMING 1
/* JPEG capture */
#define JPEG_CAPTURE   2
/** @} */ // end of uvc_app_type

/**
 * @class UVCClass UVC.h
 * @brief UVCClass is used to control an USB campera
 */
class UVCClass
{
public:

    /** @brief Constructor */
    UVCClass();

    /** @brief Destructor */
    ~UVCClass();

    /**
     * @brief Initiate and starts the UVC
     *
     * @param[in] frame_type The frame type could be either MJPEG or H.264
     * @param[in] width The width of a frame
     * @param[in] height The height of a frame
     * @param[in] frame_rate The frame rate
     * @param[in] compression_ratio Nop
     * @param[in] app_type The application can be either RTSP streaming or JPEG capture.
     */
    void begin(int frame_type, int width, int height, int frame_rate, int compression_ratio, int app_type=RTSP_STREAMING);

    /**
     * @brief Initiate and starts the UVC
     *
     * It use these default settings: \n
     *     Frame type: MJPEG\n
     *     Width: 320\n
     *     Height: 240\n
     *     Framerate: 30\n
     *     Compression ratio: 0\n
     *     Application: RTSP streaming
     */
    void begin();

    /**
     * @brief Deinitialize the UVC
     */
    void end();

    /**
     * @brief Check if UVC is ready
     *
     * @return True if UVC is ready. And false vise versa.
     */
    int available();

    /**
     * @brief Check if UVC is currently streaming
     *
     * @return True if UVC is in streaming. And false vise versa.
     */
    int status();

    /**
     * @brief Enable straming
     *
     * If the UVC is turned off stream. Call this function can turn on streaming
     *
     * @return Return 0 if success
     */
    int  turnOn();

    /**
     * @brief Disable straming
     *
     * Disble streaming without deinitialize UVC.
     */
    void turnOff();

    /**
     * @brief Get a JPEG if application type is JPEG capture
     *
     * @param[in] buf The buffer to store the JPEG
     * @return The JPEG size
     */
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

    void *msink_ctx;
    void *msrc_ctx;
};

extern UVCClass UVC;

#endif

/** @} */ // end of group UVC