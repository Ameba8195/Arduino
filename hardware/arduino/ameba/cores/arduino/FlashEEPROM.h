/*
  FlashEEPROM.h - FlashEEPROM library
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

#ifndef FlashEEPROM_h
#define FlashEEPROM_h

#include <inttypes.h>

#define flashEEPROM_size 512

class FlashEEPROMClass
{
  public:
	uint8_t data[flashEEPROM_size];
  	void read_from_flashEEPROM(void);
  	void write_to_flashEEPROM(void);
	int length(void);
};

extern FlashEEPROMClass flashEEPROM;

#endif


