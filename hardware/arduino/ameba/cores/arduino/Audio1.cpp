/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
 * Audio library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


#include "Arduino.h"

#include "Audio1.h"

#include "DAC1.h"



void AudioClass1::dump_wav_header(WAV_HEADER* pWH)
{
	char buf[5];
	
	if ( pWH == NULL ) return;
	
	rtl_printf("wave file header : \r\n");
	rtl_printf("    -- chunk ID     : 0x%x \r\n", *((uint32_t*)(&pWH->ckID[0])));
	rtl_printf("    -- chunk size   : %d \r\n", pWH->ckSize);

	memcpy(buf, pWH->ckFormat, 4);
	buf[4]=0;
	rtl_printf("    -- chunk format : %s \r\n", buf);

}

void AudioClass1::dump_wav_sub_header(WAV_SUB_HEADER* pWSH)
{
	char buf[5];
	
	if ( pWSH == NULL ) return;

	memset(buf, 0, sizeof(buf));
	
	rtl_printf("   sub header : \r\n");
	rtl_printf("    -- chunk ID     : 0x%x \r\n", *((uint32_t*)(&pWSH->ckID[0])));
	rtl_printf("    -- chunk size   : %d \r\n", pWSH->ckSize);
	rtl_printf("    -- chunk format : %x \r\n", pWSH->format);
	rtl_printf("    -- channels : %d \r\n", pWSH->channels);
	rtl_printf("    -- sample frequency : %d \r\n", pWSH->sample_freq);
	rtl_printf("    -- data per sec : %d \r\n", pWSH->data_per_sec);
	rtl_printf("    -- sub block size : %d \r\n", pWSH->sub_block_size);
	rtl_printf("    -- sub bits per sample : %d \r\n", pWSH->bits_per_sample);

	memcpy(buf, pWSH->dataStr, 4);
	rtl_printf("    -- data name : %s \r\n", buf);
	
	rtl_printf("    -- data size : %d \r\n", pWSH->data_size);

	//pAudioBuf = (uint8_t*)&wav_buf[44];
	//abuf_size = subh.data_size;


}


int AudioClass1::read_wav_buf(uint8_t *wav_buf, int wavbuf_size)
{

	int wavbuf_pos = 0;
	WAV_SUB_HEADER subh;


	rtl_printf("wav size=%d \r\n", wavbuf_size);

	memcpy(&wav_header, &wav_buf[0], sizeof(wav_header));
	wavbuf_pos += sizeof(wav_header);

	dump_wav_header(&wav_header);

	//
	// sub header
	//
	
	memcpy(&wav_sub_header, &wav_buf[wavbuf_pos], sizeof(wav_sub_header));
	wavbuf_pos += sizeof(subh);

	dump_wav_sub_header(&wav_sub_header);


	wav_chunk_size = wav_sub_header.data_size;
	wav_freq = wav_sub_header.sample_freq / 1000;

	rtl_printf("freq=%d \r\n", wav_freq);
	return wav_chunk_size;
}


static inline 
uint16_t map1(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define MAX_AUDIO_BUF_SIZE 1024
uint16_t audio_buf[MAX_AUDIO_BUF_SIZE];

static inline uint16_t transfer_from_wav8(uint8_t data)
{
	uint16_t ret;

	ret = map1(data, 0, 0xFF, 0, 0xFFF);
	return (ret);
}

void AudioClass1::play8(uint8_t *data, int size)
{
	
	if ( size > MAX_AUDIO_BUF_SIZE ) return;

	for (int i=0; i<size; i++) {
		audio_buf[i] = transfer_from_wav8(data[i]);
	}

	DAC0.send16_freq(audio_buf, size, wav_freq);
}


#if 0

http://developer.mbed.org/users/sravet/code/wave_player/file/acc3e18e77ad/wave_player.cpp


switch( wav_header.ckID ) {
	case 0x46464952:
		rtl_printf(" * RIFF \r\n");
		break;
	case 0x20746d66:
		rtl_printf(" * FORMAT chunk \r\n");
		break;
	case 0x61746164:
		rtl_printf(" * DATA chunk \r\n");
		break;
	case 0x5453494c:
		rtl_printf(" * INFO chunk \r\n");
		break;
	default:
		break;
}




void AudioClass1::prepare(int16_t *buffer, int S, int volume){
    uint16_t *ubuffer = (uint16_t*) buffer;
    for (int i=0; i<S; i++) {
        // set volume amplitude (signed multiply)
        buffer[i] = buffer[i] * volume / 1024;
        // convert from signed 16 bit to unsigned 12 bit for DAC.
        ubuffer[i] += 0x8000;
        ubuffer[i] >>= 4;
    }
}

size_t AudioClass1::write(const uint32_t *data, size_t size) {
	DAC0.send16((uint16_t)data, size, 1);
}
#endif



