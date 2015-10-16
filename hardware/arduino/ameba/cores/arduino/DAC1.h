/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
 * DAC library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef DAC1_INCLUDED
#define DAC1_INCLUDED



#ifdef __cplusplus // only for C++

#include "Arduino.h"


extern "C" {
#include "hal_dac.h"

typedef struct DACC_struct {
    PSAL_DAC_HND 	pDACVeriHnd;
	PSAL_DAC_HND_PRIV   pSalDACHNDPriv;
	PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt;
	PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt;
	PHAL_GDMA_OP		  pHALDACGdmaOp;
	PIRQ_HANDLE		  pIrqHandleDACGdma;
} DACC;

}

const int DAC_pin0 = 0;

#define MAX_DAC_BUFFER_NUM  4
#define MAX_DAC_BUFFER_SIZE 1024

//typedef void (*OnTransmitEnd_CB)(void *data);

class DACClass1
{
public:
	
	DACClass1(){};
    void begin(uint8_t data_rate);
	void send16(uint16_t* buffer, unsigned int len);
	void send16_repeat(uint16_t* buffer, unsigned int len, int repeat);	
	void send(uint32_t* buffer, unsigned int len, int repeat=0);

	void send16_freq(uint16_t* buffer, unsigned int len, unsigned int freq);
	
	void gen_sinewave(unsigned int freq, int repeat);

protected:
    void dacc_init(void);
	uint16_t MAXDACVAL = 0x7E0;
	uint16_t MINDACVAL = 0x820;

    uint16_t dac_buffer[MAX_DAC_BUFFER_NUM][MAX_DAC_BUFFER_SIZE];
    unsigned int dac_buffer_size = 0;
    unsigned int dac_buffer_page=0;
	unsigned int dac_current_pos=0;

	unsigned int transform_buffer16_to_250K(uint16_t* pBuffer, uint16_t* in_Buffer, unsigned int len, unsigned int from_bps);
	int transform_buffer16_to_10K(uint16_t* pBuffer, uint16_t* in_Buffer, unsigned int len, unsigned int from_bps);

	uint16_t transform_dac_val(uint16_t val);
	uint16_t transform_dac_val_to_bit12(uint16_t val8, uint bits);
 
	
	void calculate_sinewave(uint16_t *pBuffer, unsigned int buf_size);

private:
	uint32_t frequency;
    DACC dac;
	SAL_DAC_TRANSFER_BUF	DACTxBuf;
	//uint8_t  is_sent;

};

extern DACClass1 DAC0;

#endif // only for C++

#endif
