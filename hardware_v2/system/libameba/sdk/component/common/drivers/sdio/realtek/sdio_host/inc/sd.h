#ifndef _SD_DRIVER_H
#define _SD_DRIVER_H

#include "basic_types.h"

#define CONFIG_SD_SDIO	1
#define CONFIG_SD_SPI	0

typedef enum
{
	SD_OK = 0,
	SD_PROTECTED,
	SD_NODISK,
	SD_INITERR,
	SD_ERROR,
}SD_RESULT;

typedef enum{
	SD_CLK_LOW,		// 10.4MHz
	SD_CLK_MID,		// 20.8MHz
	SD_CLK_HIGH,	// 41.6MHz
	SD_CLK_RSV,		// 5.2MHz
}SD_CLK;

SD_RESULT SD_WaitReady(void);                        
SD_RESULT SD_Init(void);     
SD_RESULT SD_DeInit(void);   
SD_RESULT SD_SetCLK(SD_CLK CLK);

SD_RESULT SD_Status(void);

SD_RESULT SD_GetCID(u8 *cid_data);  // read sd card CID
SD_RESULT SD_GetCSD(u8 *csd_data);  // read sd card CSD
SD_RESULT SD_GetCapacity(u32* sector_count);    // read sd card Capacity

SD_RESULT SD_ReadBlocks(u32 sector,u8 *data,u32 count); //read multi sector
SD_RESULT SD_WriteBlocks(u32 sector,const u8 *data,u32 count);  //write multi sector

#endif
