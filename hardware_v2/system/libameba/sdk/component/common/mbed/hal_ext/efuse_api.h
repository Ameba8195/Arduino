/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#ifndef MBED_EXT_EFUSE_API_EXT_H
#define MBED_EXT_EFUSE_API_EXT_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

int  efuse_get_remaining_length(void);
void efuse_mtp_read(uint8_t * data);
int  efuse_mtp_write(uint8_t *data, uint8_t len);
int  efuse_otp_read(u8 address, u8 len, u8 *buf);
int  efuse_otp_write(u8 address, u8 len, u8 *buf);
int  efuse_disable_jtag(void);

#ifdef __cplusplus
}
#endif

#endif // MBED_EXT_EFUSE_API_EXT_H
