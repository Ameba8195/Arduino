#ifndef _H264_H
#define _H264_H

#include "dlist.h"      //list management
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"

#define H264_DEBUG 0

#if H264_DEBUG
#define H264_PRINTF(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define H264_ERROR(fmt, args...)     printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#else
#define H264_PRINTF(fmt, args...)    
#define H264_ERROR(fmt, args...)     
#endif

/* in favor of rtp fragmentation */
struct rtp_h264_obj
{
        u8 start_code_len;
        unsigned char nal_header;
        u8 is_fu_start;
        u8 is_fu_end;
};


/********************************h264 over rtp******************************/


#endif /*_H264_H*/