/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_SDIO_HOST_H_
#define _HAL_SDIO_HOST_H_


#include "rtl8195a_sdio_host.h"



#define SDIO_HOST_WAIT_FOREVER       0xFFFFFFFF



typedef struct _HAL_SDIO_HOST_OP_ {
	HAL_Status	(*HalSdioHostInitHost)			(VOID *Data);
	HAL_Status	(*HalSdioHostInitCard)			(VOID *Data);
	HAL_Status	(*HalSdioHostDeInit)			(VOID *Data);
	HAL_Status	(*HalSdioHostRegIrq)			(VOID *Data);
	HAL_Status	(*HalSdioHostReadBlocksDma)		(VOID *Data, u64 ReadAddr, u32 BlockCnt);
	HAL_Status	(*HalSdioHostWriteBlocksDma)	(VOID *Data, u64 WriteAddr, u32 BlockCnt);
	HAL_Status	(*HalSdioHostStopTransfer)		(VOID *Data);
	HAL_Status	(*HalSdioHostGetCardStatus) 	(VOID *Data);
	HAL_Status	(*HalSdioHostGetSdStatus) 		(VOID *Data);
    HAL_Status  (*HalSdioHostChangeSdClock)     (VOID *Data, u8 Frequency);
	HAL_Status	(*HalSdioHostErase)				(VOID *Data, u64 StartAddr, u64 EndAddr);
	HAL_Status	(*HalSdioHostGetWriteProtect)	(VOID *Data);
	HAL_Status	(*HalSdioHostSetWriteProtect)	(VOID *Data, u8 Setting);
}HAL_SDIO_HOST_OP, *PHAL_SDIO_HOST_OP;

// SDIO error type 
typedef enum _SDIO_ERR_TYPE_ {
    SDIO_ERR_DAT_CRC    =   0x01,
    SDIO_ERR_CMD_TIMEOUT    =   0x02,
}SDIO_ERR_TYPE;

typedef enum _SDIO_XFER_TYPE_{
	SDIO_XFER_NOR	= 0x00, // normal 
	SDIO_XFER_R	= 0x01,	// read and write block
	SDIO_XFER_W	= 0x02,	// read and write block
}SDIO_XFER_TYPE;

typedef struct _HAL_SDIO_HOST_ADAPTER_{
	IRQ_HANDLE				IrqHandle;			// Irq Handler
	ADMA2_DESC_FMT			*AdmaDescTbl;
	u32						Response[4];	
	u32						CardOCR;
	u32 					CardStatus;
	u32						IsWriteProtect;
	u8 						SdStatus[SD_STATUS_LEN];
	u8						Csd[CSD_REG_LEN];
    volatile u8             CmdCompleteFlg;
    volatile u8             XferCompleteFlg;
	volatile u8             ErrIntFlg;
    volatile u8             CardCurState;
	u8						IsSdhc;
	u8						CurrSdClk;
	u16 					RCA;
	u16						SdSpecVer;
	SDIO_ERR_TYPE			errType;
	SDIO_XFER_TYPE			XferType;
	VOID (*XferCompCallback)(VOID *pAdapter);
	VOID *XferCompCbPara;
	VOID (*ErrorCallback)(VOID *pAdapter);
	VOID *ErrorCbPara;
	VOID (*CardInsertCallBack)(VOID *pAdapter);
	VOID *CardInsertCbPara;
	VOID (*CardRemoveCallBack)(VOID *pAdapter);
	VOID *CardRemoveCbPara;
}HAL_SDIO_HOST_ADAPTER, *PHAL_SDIO_HOST_ADAPTER;


extern HAL_Status 
HalSdioHostInit(
	IN VOID *Data
);

extern HAL_Status 
HalSdioHostDeInit(
	IN VOID *Data
);

extern HAL_Status 
HalSdioHostEnable(
	IN VOID *Data
);

extern HAL_Status 
HalSdioHostDisable(
	IN VOID *Data
);

extern VOID
HalSdioHostOpInit(
	IN VOID *Data
);


#endif

