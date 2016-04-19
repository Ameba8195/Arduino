#ifndef _AV_CODEC_H_
#define _AV_CODEC_H_

/* Is this the place to include different codec header for rtp usage?*/


struct codec_info
{
       int codec_id;
       const char codec_name[8];
       u8 pt;
       u32 clock_rate;
       u8 audio_channels;
       void *priv;
};

/* media type list -- range from 0-255 stored in 1 BYTE*/
#define AVMEDIA_TYPE_VIDEO 0
#define AVMEDIA_TYPE_AUDIO 1
#define AVMEDIA_TYPE_SUBTITLE  2
#define AVMEDIA_TYPE_UNKNOWN 255

/*codec id list -- id must match its placing order (starting from 0) in av_codec_tables*/

#define AV_CODEC_ID_MJPEG 0
#define AV_CODEC_ID_H264  1
#define AV_CODEC_ID_PCMU  2
#define AV_CODEC_ID_PCMA  3
#define AV_CODEC_ID_UNKNOWN -1

/*rtp payload type mapping and standard rtp payload type table -- range from 0-255 in 1 BYTE*/
#define RTP_PT_PCMU     0
#define RTP_PT_GSM      3
#define RTP_PT_G723     4
#define RTP_PT_DVI4_R8000        5
#define RTP_PT_DVI4_R16000       6
#define RTP_PT_LPC      7
#define RTP_PT_PCMA     8
#define RTP_PT_G722     9
#define RTP_PT_L16_C2   10
#define RTP_PT_L16_C1   11
#define RTP_PT_QCELP    12
#define RTP_PT_CN       13
#define RTP_PT_MPA      14
#define RTP_PT_G728     15
#define RTP_PT_DVI4_R11025      16
#define RTP_PT_DVI4_R22050      17
#define RTP_PT_G719     18
#define RTP_PT_CELB     25
#define RTP_PT_JPEG     26
#define RTP_PT_NV       28
#define RTP_PT_H261     31
#define RTP_PT_MPV      32
#define RTP_PT_MP2T     33
#define RTP_PT_H263     34
#define RTP_PT_RTCP_BASE        72
#define RTP_PT_DYN_BASE         96
#define RTP_PT_UNKNOWN          255

/*av codec tables*/
//#include "section_config.h"
//SDRAM_DATA_SECTION
static const struct codec_info av_codec_tables[] = {
  {AV_CODEC_ID_MJPEG, "MJPEG", RTP_PT_JPEG, 90000, 0, 0},
  {AV_CODEC_ID_H264, "H264", RTP_PT_DYN_BASE, 90000, 0, 0},
  {AV_CODEC_ID_PCMU, "PCMU", RTP_PT_PCMU, 8000, 1, 0},
  {AV_CODEC_ID_PCMA, "PCMA", RTP_PT_PCMA, 8000, 1, 0}
};
#define AVCODEC_SIZE 4

void get_codec_by_id(struct codec_info *c, int id);

#endif  //_AV_CODEC_H_