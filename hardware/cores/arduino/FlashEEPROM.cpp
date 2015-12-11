/*
  FlashEEPROM.cpp - FlashEEPROM library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "Arduino.h"
#include "FlashEEPROM.h"
#include "flash_api.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/

/******************************************************************************
 * User API
 ******************************************************************************/

static uint32_t flashEEPROM_start_pos = 0x200000-flashEEPROM_size;

void FlashEEPROMClass::read_from_flashEEPROM(void)
{
   int address =0;
   while (address < flashEEPROM_size) {
	   flash_read_word(&flash, flashEEPROM_start_pos+address, (uint32_t*)(&data[address]));
	   address = address + 4;
   }
}

void FlashEEPROMClass::write_to_flashEEPROM(void)
{
   int address =0;
   flash_erase_sector(&flash, flashEEPROM_start_pos);
   while (address < flashEEPROM_size) {
	   flash_write_word(&flash, flashEEPROM_start_pos+address, *((uint32_t*)(&data[address])));
	   address = address + 4;
   }
}


int FlashEEPROMClass::length(void)
{
	return flashEEPROM_size;
}

FlashEEPROMClass flashEEPROM;

