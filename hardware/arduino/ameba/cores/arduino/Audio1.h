/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
 * Audio library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef AUDIO1_H
#define AUDIO1_H



#include "Arduino.h"
#include "DAC1.h"

#ifndef DWORD
typedef uint32_t DWORD;
#endif

#ifndef BYTE
typedef uint8_t BYTE;
#endif

typedef struct { 
     BYTE ckID[4];        // The unique chunk identifier 
     DWORD ckSize;       // The size of field  
     BYTE ckFormat[4];
} WAV_HEADER; 

typedef struct { 
     BYTE ckID[4];        // The unique chunk identifier 
     DWORD ckSize;       // The size of field  
     uint16_t format;
     uint16_t channels;
     DWORD sample_freq;
     DWORD data_per_sec;
     uint16_t sub_block_size;
     uint16_t bits_per_sample;
     BYTE dataStr[4];        // The unique chunk identifier 
     DWORD data_size;       // The size of field  
} WAV_SUB_HEADER; 

class AudioClass1 {

public:	
	AudioClass1() { DAC0.begin(DAC_DATA_RATE_10K); };
	int read_wav_buf(uint8_t *wav_buf, int wavbuf_size);
	//uint16_t* get_wav_chunk_buf();
	int get_wav_chunk_size() { return wav_chunk_size; }
	void play8(uint8_t *data, int size);

protected:
	WAV_HEADER 		wav_header;	
	WAV_SUB_HEADER 	wav_sub_header;

	void dump_wav_header(WAV_HEADER* pWH);
	void dump_wav_sub_header(WAV_SUB_HEADER* pWSH);
	//uint16_t* wav_chunk_buf; 
	int wav_chunk_size;
	int wav_freq;
};


#endif 

