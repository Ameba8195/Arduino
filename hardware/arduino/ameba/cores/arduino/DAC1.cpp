/*
 * Copyright (c) 2012 by Cristian Maglie <c.maglie@bug.st>
 * DAC library for Arduino Due.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <Arduino.h>
#include <DAC1.h>



extern "C" {

#include "hal_dac.h"
#include "rt_os_service.h"

#include "section_config.h"

}


void DACClass1::dacc_init() {
	
    this->dac.pDACVeriHnd = NULL;
    this->dac.pSalDACHNDPriv = NULL;
	this->dac.pSalDACMngtAdpt = NULL;
	this->dac.pHALDACGdmaAdpt = NULL;
	this->dac.pHALDACGdmaOp = NULL;
	this->dac.pIrqHandleDACGdma = NULL;
}

void DACClass1::begin(uint8_t data_rate) 
{
	int pin = DAC_pin0;


	// init DAC
	
    PSAL_DAC_HND        pSalDACHND      = NULL;


	this->frequency = (data_rate == DAC_DATA_RATE_10K )? 10000 : 25000;
	
    dacc_init();

    /* Check the user define setting and the given index */
    if (RtkDACIdxChk(pin)) {
        return;
    }

    /* Invoke RtkI2CGetMngtAdpt to get the I2C SAL management adapter pointer */
    this->dac.pSalDACMngtAdpt = RtkDACGetMngtAdpt(pin);

    /* Assign the private SAL handle to public SAL handle */
    pSalDACHND      = &(this->dac.pSalDACMngtAdpt->pSalHndPriv->SalDACHndPriv);
    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalDACHND->pInitDat    = this->dac.pSalDACMngtAdpt->pHalInitDat;
    
    this->dac.pDACVeriHnd = &(this->dac.pSalDACMngtAdpt->pSalHndPriv->SalDACHndPriv);

	//

	this->dac.pDACVeriHnd->DevNum	  = pin;
	RtkDACLoadDefault(this->dac.pDACVeriHnd);
	this->dac.pDACVeriHnd->pInitDat->DACDataRate	  =   data_rate;
	//this->dac.pDACVeriHnd->pInitDat->DACDataRate	  =   DAC_DATA_RATE_250K;
	//this->dac.pDACVeriHnd->pInitDat->DACDataRate	  =   DAC_DATA_RATE_10K;
	this->dac.pDACVeriHnd->pInitDat->DACEndian		  =   DAC_DATA_ENDIAN_LITTLE;
	this->dac.pDACVeriHnd->pInitDat->DACBurstSz 	  =   10;

	/* To Get the SAL_I2C_MNGT_ADPT Pointer */
	pSalDACHND = this->dac.pDACVeriHnd;
	this->dac.pSalDACHNDPriv  = CONTAINER_OF(this->dac.pDACVeriHnd, SAL_DAC_HND_PRIV, SalDACHndPriv);
	this->dac.pSalDACMngtAdpt = CONTAINER_OF(this->dac.pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);

	this->dac.pHALDACGdmaAdpt	  = this->dac.pSalDACMngtAdpt->pHalGdmaAdp;
	this->dac.pHALDACGdmaOp 	  = this->dac.pSalDACMngtAdpt->pHalGdmaOp;  
	this->dac.pIrqHandleDACGdma   = this->dac.pSalDACMngtAdpt->pIrqGdmaHnd;

	RtkDACInit(this->dac.pDACVeriHnd);
	this->dac.pHALDACGdmaAdpt->MuliBlockCunt	  = 2;
	this->dac.pHALDACGdmaAdpt->MaxMuliBlock 	  = 2;//MaxLlp;

	//this->is_sent = _FALSE;
	this->dac_current_pos = 0;
}




unsigned int DACClass1::transform_buffer16_to_250K(uint16_t* pBuffer, uint16_t* in_Buffer, unsigned int len, unsigned int from_bps) {
	
    unsigned int new_len;
	double steps, val1, val2, pre_val;
	unsigned int new_len_max = len * 250 / from_bps ;
	unsigned int pre_pos =0, new_pos=0, in_pos=0; 

    if ( len <= 0 ) return 0;

	val1 = (double)(in_Buffer[0]);
	if ( len == 1 ) {
		pBuffer[0] = transform_dac_val(in_Buffer[0]);	
		return 1;
	}
	
	val2 = (double)(in_Buffer[1]);
	pre_pos = 0;
	steps = (val2-val1)*(double)(from_bps)/250.0; 
	pre_val = val1;
	for (int i = 1; i < new_len_max-1; i++) {
	   in_pos = i * from_bps / 250;
	   if ( in_pos == pre_pos ) {
	   	 pre_val = pre_val+steps;
	   } else {
		   val1 = (double)(in_Buffer[pre_pos]);
		   val2 = (double)(in_Buffer[in_pos]);
		   steps = (val2-val1)*(double)(from_bps)/250.0; 
		   pre_val = val1;
		   pre_pos = in_pos;
	   }
	   pBuffer[i] = transform_dac_val((uint16_t)round(pre_val));
	}
	
	pBuffer[new_len_max-1] = transform_dac_val((uint16_t)round(val2));	
	return new_len_max;
}


int DACClass1::transform_buffer16_to_10K(uint16_t* pBuffer, uint16_t* in_Buffer, unsigned int len, unsigned int from_bps) 
{
	uint16_t val;

	int new_len = len * 10 / from_bps;
	
    if ( len <= 0 ) return 0;

	val = in_Buffer[0];
	if ( len == 1 ) {
		pBuffer[0] = transform_dac_val(val);	
		return 1;
	}

	for (int i=0; i<new_len; i++) {
		pBuffer[i] = transform_dac_val(in_Buffer[i*from_bps/10]);
	}
	return new_len;	
}



void DACClass1::send16_freq(uint16_t* buffer, unsigned int len, unsigned int freq)
{

	uint16_t *pBuffer;
	int new_len;

	// handle buffer 
	pBuffer = (uint16_t*)(&dac_buffer[dac_buffer_page][0]);
	new_len = transform_buffer16_to_10K(pBuffer, buffer, len, freq);
	send((uint32_t*)pBuffer, new_len);
	dac_buffer_page = ( dac_buffer_page + 1 ) % MAX_DAC_BUFFER_NUM;
}


void DACClass1::send16(uint16_t* buffer, unsigned int len)
{
	uint16_t *pBuffer;

	for (int i=0; i< len ; i++ ) {
		pBuffer = (uint16_t*)(&dac_buffer[dac_buffer_page][dac_current_pos++]);
		*pBuffer = transform_dac_val(buffer[i]);
	}
	
	send((uint32_t*)&dac_buffer[dac_buffer_page][0], dac_current_pos);
	dac_current_pos = 0;
	dac_buffer_page = ( dac_buffer_page + 1 ) % MAX_DAC_BUFFER_NUM;

}

void DACClass1::send16_repeat(uint16_t* buffer, unsigned int len, int repeat)
{
	uint16_t *pBuffer;

	if ( len > MAX_DAC_BUFFER_SIZE ) {
		DiagPrintf("%s : ERROR len(%d) > MAX_DAC_BUFFER_SIZE(%d) \r\n", __FUNCTION__, len, MAX_DAC_BUFFER_SIZE );
		return;
	}
	
	pBuffer = (uint16_t*)(&dac_buffer[dac_buffer_page][0]);
	for (int i=0; i< len ; i++ ) {
		pBuffer[i] = transform_dac_val(buffer[i]);
	}
	send((uint32_t*)pBuffer, len, repeat);

	dac_buffer_page = ( dac_buffer_page + 1 ) % MAX_DAC_BUFFER_NUM;
	
}


void DACClass1::send(uint32_t* buffer, unsigned int len, int repeat)
{
	PSAL_DAC_HND	    pSalDACHND;  
	PHAL_GDMA_ADAPTER	pHALDACGdmaAdpt;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt;	
	int count = 0;
	PSAL_DAC_USER_CB pUserCB;
	static int is_init = _FALSE;

	pSalDACHND = this->dac.pDACVeriHnd;
	pHALDACGdmaAdpt = this->dac.pHALDACGdmaAdpt;
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);

			
	if ( is_init == _FALSE ) { // first sent


		pSalDACMngtAdpt->pUserCB = (PSAL_DAC_USER_CB)rtw_malloc(sizeof(SAL_DAC_USER_CB));
		pUserCB = pSalDACMngtAdpt->pUserCB;
		DiagPrintf(" DAC %s pUserCB : 0x%x \r\n", __FUNCTION__, pUserCB);
		pUserCB->dataLen = len/2;
		pUserCB->pBuf = buffer;
		pUserCB->repeat = repeat;
		pUserCB->next = (PSAL_DAC_USER_CB)rtw_malloc(sizeof(SAL_DAC_USER_CB));
		pUserCB = pUserCB->next;

		for (int i=1; i<MAX_DAC_BUFFER_NUM-1; i++) {
			DiagPrintf(" DAC %s pUserCB : 0x%x \r\n", __FUNCTION__, pUserCB);
			pUserCB->pBuf = NULL;
			pUserCB->next = (PSAL_DAC_USER_CB)rtw_malloc(sizeof(SAL_DAC_USER_CB));
			pUserCB = pUserCB->next;
		}

		DiagPrintf(" DAC %s pUserCB : 0x%x \r\n", __FUNCTION__, pUserCB);
		pUserCB->pBuf = NULL;
		pUserCB->next =	pSalDACMngtAdpt->pUserCB;
		pSalDACMngtAdpt->pUserCB_head = pSalDACMngtAdpt->pUserCB;
		pSalDACMngtAdpt->pUserCB_tail = pSalDACMngtAdpt->pUserCB->next;


		pUserCB = pSalDACMngtAdpt->pUserCB_head;
		
		pSalDACHND->pTXBuf			= &this->DACTxBuf;
		pSalDACHND->pTXBuf->DataLen = pUserCB->dataLen;
		pSalDACHND->pTXBuf->pDataBuf	= pUserCB->pBuf;
		
		pHALDACGdmaAdpt->MuliBlockCunt		= 1;
		pHALDACGdmaAdpt->MaxMuliBlock		= 2;//MaxLlp;
		
		RtkDACSend(pSalDACHND);
		pSalDACMngtAdpt->isSent=1;
		is_init = _TRUE;

	}
	else {
		 int i;
		 PSAL_DAC_USER_CB pUserCB; 

		 pUserCB = pSalDACMngtAdpt->pUserCB_tail;
		 if (pUserCB->pBuf != NULL ) {
			 while (1) {
			 	cli();
			 	if ( pUserCB->pBuf == NULL ) {
					sti();
					break;
			 	}
				sti();
			 	delay(1);				
			 }

		 }

		 
		 cli();
		 pUserCB->dataLen = len/2;
		 pUserCB->pBuf = buffer; 
		 pUserCB->repeat = repeat;
		 pSalDACMngtAdpt->pUserCB_tail = pUserCB->next;
		 sti(); 		 

		if ( pSalDACMngtAdpt->isSent == 0 ) {
			pUserCB = pSalDACMngtAdpt->pUserCB_head;
			
			pSalDACHND->pTXBuf			= &this->DACTxBuf;
			pSalDACHND->pTXBuf->DataLen = pUserCB->dataLen;
			pSalDACHND->pTXBuf->pDataBuf	= pUserCB->pBuf;
			
			pHALDACGdmaAdpt->MuliBlockCunt		= 1;
			pHALDACGdmaAdpt->MaxMuliBlock		= 2;//MaxLlp;
			
			RtkDACSend(pSalDACHND);
			pSalDACMngtAdpt->isSent=1;			
		}
	}
}



uint16_t DACClass1::transform_dac_val(uint16_t val) 
{
	 if ( val >= 0x800 ) {
		   val -= 0x800;
		   if ( val > this->MAXDACVAL ) val = this->MAXDACVAL;
	 } else {
		   val += 0x800;
		   if ( val < this->MINDACVAL ) val = this->MINDACVAL;
	 }
	
	return val;   
}

uint16_t DACClass1::transform_dac_val_to_bit12(uint16_t val8, uint bits) 
{
    uint16_t val;

	if ( bits < 8 ) {
		return 0;
	} else if ( bits <= 12) {
		val = (val8 << (12-bits));
	} else if ( bits <= 16 ) {
		val = (val8 >> (bits-12));
	} else {
		return 0;
	}

	return transform_dac_val(val);       
}

#define PI        (3.141592653589793238462)
#define AMPLITUDE (1.0)    // x * 3.3V
#define PHASE     (PI * 1) // 2*pi is one period
#define RANGE     (4096/2) // 12 bits DAC
#define OFFSET    (4096/2) // 12 bits DAC


void DACClass1::calculate_sinewave(uint16_t *pBuffer, unsigned int buf_size)
{
  for (int i = 0; i < buf_size; i++) {
     double rads = (2*PI * i)/buf_size; // Convert degree in radian
     double val;

	 val = ((double)(RANGE) * (cos(rads + PHASE))) + (double)(OFFSET);
     pBuffer[i] = (uint16_t)val;
     //rtl_printf("0x%x ", buffer[i]);
  }
}


static uint16_t gen_buffer[MAX_DAC_BUFFER_SIZE];
void DACClass1::gen_sinewave(unsigned int freq, int repeat)
{
	int buffer_size;

	if (freq <=0 ) {
		DiagPrintf(" %s : ERROR : frequency=%d \r\n", __FUNCTION__, freq);
		return;
	}

	buffer_size = this->frequency / freq ;  
	if ( buffer_size > sizeof(gen_buffer) ) buffer_size = sizeof(gen_buffer);
	if ( buffer_size <= 0 ) buffer_size = this->frequency/1000;

	calculate_sinewave(&gen_buffer[0], buffer_size);
	
	if ( repeat < 1 ) repeat = 1;
	send16_repeat(&gen_buffer[0], buffer_size, repeat);

}


