/*
Copyright (c) 2007, Jim Studt  (original old version - many contributors since)

The latest version of this library may be found at:
  http://www.pjrc.com/teensy/td_libs_OneWire.html

OneWire has been maintained by Paul Stoffregen (paul@pjrc.com) since
January 2010.  At the time, it was in need of many bug fixes, but had
been abandoned the original author (Jim Studt).  None of the known
contributors were interested in maintaining OneWire.  Paul typically
works on OneWire every 6 to 12 months.  Patches usually wait that
long.  If anyone is interested in more actively maintaining OneWire,
please contact Paul.

Version 2.2:
  Teensy 3.0 compatibility, Paul Stoffregen, paul@pjrc.com
  Arduino Due compatibility, http://arduino.cc/forum/index.php?topic=141030
  Fix DS18B20 example negative temperature
  Fix DS18B20 example's low res modes, Ken Butcher
  Improve reset timing, Mark Tillotson
  Add const qualifiers, Bertrik Sikken
  Add initial value input to crc16, Bertrik Sikken
  Add target_search() function, Scott Roberts

Version 2.1:
  Arduino 1.0 compatibility, Paul Stoffregen
  Improve temperature example, Paul Stoffregen
  DS250x_PROM example, Guillermo Lovato
  PIC32 (chipKit) compatibility, Jason Dangel, dangel.jason AT gmail.com
  Improvements from Glenn Trewitt:
  - crc16() now works
  - check_crc16() does all of calculation/checking work.
  - Added read_bytes() and write_bytes(), to reduce tedious loops.
  - Added ds2408 example.
  Delete very old, out-of-date readme file (info is here)

Version 2.0: Modifications by Paul Stoffregen, January 2010:
http://www.pjrc.com/teensy/td_libs_OneWire.html
  Search fix from Robin James
    http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27
  Use direct optimized I/O in all cases
  Disable interrupts during timing critical sections
    (this solves many random communication errors)
  Disable interrupts during read-modify-write I/O
  Reduce RAM consumption by eliminating unnecessary
    variables and trimming many to 8 bits
  Optimize both crc8 - table version moved to flash

Modified to work with larger numbers of devices - avoids loop.
Tested in Arduino 11 alpha with 12 sensors.
26 Sept 2008 -- Robin James
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27

Updated to work with arduino-0008 and to include skip() as of
2007/07/06. --RJL20

Modified to calculate the 8-bit CRC directly, avoiding the need for
the 256-byte lookup table to be loaded in RAM.  Tested in arduino-0010
-- Tom Pollard, Jan 23, 2008

Jim Studt's original library was modified by Josh Larios.

Tom Pollard, pollard@alum.mit.edu, contributed around May 20, 2008

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Much of the code was inspired by Derek Yerger's code, though I don't
think much of that remains.  In any event that was..
    (copyleft) 2006 by Derek Yerger - Free to distribute freely.

The CRC code was excerpted and inspired by the Dallas Semiconductor
sample code bearing this copyright.
//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//--------------------------------------------------------------------------
*/

#include "OneWire.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include "PinNames.h"
#include "objects.h"
#include "hal_api.h"
#include "rtl8195a_gpio.h"

#define HAL_GPIO_PIN_NAME(port,pin)         (((port)<<5)|(pin))
#define HAL_GPIO_GET_PORT_BY_NAME(x)        ((x>>5) & 0x03)
#define HAL_GPIO_GET_PIN_BY_NAME(x)         (x & 0x1f)


#ifdef __cplusplus
}
#endif


//
// private functions
static const u8 _GPIO_SWPORT_DR_TBL[] = {
    GPIO_PORTA_DR,
    GPIO_PORTB_DR,
    GPIO_PORTC_DR
};


static const u8 _GPIO_EXT_PORT_TBL[] = {
    GPIO_EXT_PORTA,
    GPIO_EXT_PORTB,
    GPIO_EXT_PORTC
};

static const PinName pin_name_tbl[] = {
	PA_6, PA_7, PA_5, PD_4, PD_5, PA_4, PA_3, PA_2, PB_4, PB_5, PC_0, PC_2, PC_3, PC_1
};

static const u8 _GPIO_PinMap_Chip2IP_8195a[][2] = {
/* Chip Pin, IP Pin */
    {_PA_0, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 0)},
    {_PA_1, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 1)},
    {_PA_2, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 0)},
    {_PA_3, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 1)},
    {_PA_4, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 2)},
    {_PA_5, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 3)},
    {_PA_6, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 4)},
    {_PA_7, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 5)},
    {_PB_0, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 6)},
    {_PB_1, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 7)},
    {_PB_2, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 8)},
    {_PB_3, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 2)},
    {_PB_4, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 3)},
    {_PB_5, HAL_GPIO_PIN_NAME(GPIO_PORT_B, 9)},
    {_PB_6, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 4)},
    {_PB_7, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 5)},
    {_PC_0, HAL_GPIO_PIN_NAME(GPIO_PORT_B,10)},
    {_PC_1, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 6)},
    {_PC_2, HAL_GPIO_PIN_NAME(GPIO_PORT_B,11)},
    {_PC_3, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 7)},
    {_PC_4, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 8)},
    {_PC_5, HAL_GPIO_PIN_NAME(GPIO_PORT_A, 9)},
    {_PC_6, HAL_GPIO_PIN_NAME(GPIO_PORT_A,10)},
    {_PC_7, HAL_GPIO_PIN_NAME(GPIO_PORT_A,11)},
    {_PC_8, HAL_GPIO_PIN_NAME(GPIO_PORT_A,12)},
    {_PC_9, HAL_GPIO_PIN_NAME(GPIO_PORT_A,13)},
    {_PD_0, HAL_GPIO_PIN_NAME(GPIO_PORT_B,12)},
    {_PD_1, HAL_GPIO_PIN_NAME(GPIO_PORT_A,14)},
    {_PD_2, HAL_GPIO_PIN_NAME(GPIO_PORT_B,13)},
    {_PD_3, HAL_GPIO_PIN_NAME(GPIO_PORT_A,15)},
    {_PD_4, HAL_GPIO_PIN_NAME(GPIO_PORT_A,16)},
    {_PD_5, HAL_GPIO_PIN_NAME(GPIO_PORT_A,17)},
    {_PD_6, HAL_GPIO_PIN_NAME(GPIO_PORT_A,18)},
    {_PD_7, HAL_GPIO_PIN_NAME(GPIO_PORT_A,19)},
    {_PD_8, HAL_GPIO_PIN_NAME(GPIO_PORT_B,14)},
    {_PD_9, HAL_GPIO_PIN_NAME(GPIO_PORT_A,20)},
    {_PE_0, HAL_GPIO_PIN_NAME(GPIO_PORT_B,15)},
    {_PE_1, HAL_GPIO_PIN_NAME(GPIO_PORT_A,21)},
    {_PE_2, HAL_GPIO_PIN_NAME(GPIO_PORT_A,22)},
    {_PE_3, HAL_GPIO_PIN_NAME(GPIO_PORT_A,23)},
    {_PE_4, HAL_GPIO_PIN_NAME(GPIO_PORT_B,16)},
    {_PE_5, HAL_GPIO_PIN_NAME(GPIO_PORT_A,24)},
    {_PE_6, HAL_GPIO_PIN_NAME(GPIO_PORT_A,25)},
    {_PE_7, HAL_GPIO_PIN_NAME(GPIO_PORT_A,26)},
    {_PE_8, HAL_GPIO_PIN_NAME(GPIO_PORT_A,27)},
    {_PE_9, HAL_GPIO_PIN_NAME(GPIO_PORT_B,17)},
    {_PE_A, HAL_GPIO_PIN_NAME(GPIO_PORT_B,18)},
    {_PF_0, HAL_GPIO_PIN_NAME(GPIO_PORT_B,19)},
    {_PF_1, HAL_GPIO_PIN_NAME(GPIO_PORT_B,20)},
    {_PF_2, HAL_GPIO_PIN_NAME(GPIO_PORT_B,21)},
    {_PF_3, HAL_GPIO_PIN_NAME(GPIO_PORT_B,22)},
    {_PF_4, HAL_GPIO_PIN_NAME(GPIO_PORT_B,23)},
    {_PF_5, HAL_GPIO_PIN_NAME(GPIO_PORT_B,24)},
    {_PG_0, HAL_GPIO_PIN_NAME(GPIO_PORT_B,25)},
    {_PG_1, HAL_GPIO_PIN_NAME(GPIO_PORT_B,26)},
    {_PG_2, HAL_GPIO_PIN_NAME(GPIO_PORT_B,27)},
    {_PG_3, HAL_GPIO_PIN_NAME(GPIO_PORT_A,28)},
    {_PG_4, HAL_GPIO_PIN_NAME(GPIO_PORT_B,28)},
    {_PG_5, HAL_GPIO_PIN_NAME(GPIO_PORT_B,29)},
    {_PG_6, HAL_GPIO_PIN_NAME(GPIO_PORT_B,30)},
    {_PG_7, HAL_GPIO_PIN_NAME(GPIO_PORT_B,31)},
    {_PH_0, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 0)},
    {_PH_1, HAL_GPIO_PIN_NAME(GPIO_PORT_A,29)},
    {_PH_2, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 1)},
    {_PH_3, HAL_GPIO_PIN_NAME(GPIO_PORT_A,30)},
    {_PH_4, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 2)},
    {_PH_5, HAL_GPIO_PIN_NAME(GPIO_PORT_A,31)},
    {_PH_6, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 3)},
    {_PH_7, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 4)},
    {_PI_0, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 5)},
    {_PI_1, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 6)},
    {_PI_2, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 7)},
    {_PI_3, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 8)},
    {_PI_4, HAL_GPIO_PIN_NAME(GPIO_PORT_C, 9)},
    {_PI_5, HAL_GPIO_PIN_NAME(GPIO_PORT_C,10)},
    {_PI_6, HAL_GPIO_PIN_NAME(GPIO_PORT_C,11)},
    {_PI_7, HAL_GPIO_PIN_NAME(GPIO_PORT_C,12)},
    {_PJ_0, HAL_GPIO_PIN_NAME(GPIO_PORT_C,13)},
    {_PJ_1, HAL_GPIO_PIN_NAME(GPIO_PORT_C,14)},
    {_PJ_2, HAL_GPIO_PIN_NAME(GPIO_PORT_C,15)},
    {_PJ_3, HAL_GPIO_PIN_NAME(GPIO_PORT_C,16)},
    {_PJ_4, HAL_GPIO_PIN_NAME(GPIO_PORT_C,17)},
    {_PJ_5, HAL_GPIO_PIN_NAME(GPIO_PORT_C,18)},
    {_PJ_6, HAL_GPIO_PIN_NAME(GPIO_PORT_C,19)},
    {_PK_0, HAL_GPIO_PIN_NAME(GPIO_PORT_C,20)},
    {_PK_1, HAL_GPIO_PIN_NAME(GPIO_PORT_C,21)},
    {_PK_2, HAL_GPIO_PIN_NAME(GPIO_PORT_C,22)},
    {_PK_3, HAL_GPIO_PIN_NAME(GPIO_PORT_C,23)},
    {_PK_4, HAL_GPIO_PIN_NAME(GPIO_PORT_C,24)},
    {_PK_5, HAL_GPIO_PIN_NAME(GPIO_PORT_C,25)},
    {_PK_6, HAL_GPIO_PIN_NAME(GPIO_PORT_C,26)},

    {0xff, 0xff}    // the end of table
};

//
uint32_t OneWire::get_pin_name(uint32_t chip_pin)
{
	uint32_t ip_pin = 0xff;
	int i;
	
    for (i=0; _GPIO_PinMap_Chip2IP_8195a[i][1] != 0xff; i++) {
        if (_GPIO_PinMap_Chip2IP_8195a[i][0] == chip_pin) {
            ip_pin = _GPIO_PinMap_Chip2IP_8195a[i][1];
            break;
        }
    }

	return ip_pin;
	
}


uint8_t OneWire::direct_read()
{
	return (uint8_t)digitalRead(this->pin);
}


uint8_t OneWire::direct_write(uint8_t pin_state)
{

	digitalWrite(this->pin, pin_state);
}



OneWire::OneWire(uint8_t pin1)
{
	PinName pin_name;

	DiagPrintf("OneWire : pin = %d \r\n", pin1);
	this->pin = pin1;
	
	pinMode(this->pin, INPUT_PULLUP);

	pin_name = pin_name_tbl[this->pin];
	this->pin_num = get_pin_name(pin_name);
    this->port_num = HAL_GPIO_GET_PORT_BY_NAME(this->pin_num);
	this->pin_addr = _GPIO_EXT_PORT_TBL[port_num];
	DiagPrintf("OneWire : pin = 0x%x, pin_num = 0x%x, port_num = 0x%x, pin_addr = 0x%x \r\n", 
		this->pin, this->pin_num, this->port_num, this->pin_addr);
	
#if ONEWIRE_SEARCH
	reset_search();
#endif
}


// Perform the onewire reset function.  We will wait up to 250uS for
// the bus to come high, if it doesn't then it is broken or shorted
// and we return a 0;
//
// Returns 1 if a device asserted a presence pulse, 0 otherwise.
//
uint8_t OneWire::reset(void)
{
	uint8_t r;
	uint32_t retries = 125;

	noInterrupts();	
	pinMode(this->pin, INPUT);
	interrupts();
	
	// wait until the wire is high... just in case
	do {
		if (--retries == 0) {
			DiagPrintf("%s timeout \r\n", __FUNCTION__);
			return 0;
		}
		delayMicroseconds(2);
	} while ( !direct_read());

	noInterrupts();	
	digital_change_dir(this->pin,OUTPUT);
	direct_write(0);
	interrupts();
	delayMicroseconds(468); // 12 us 
	noInterrupts();
	digital_change_dir(this->pin,INPUT); //8 us
	delayMicroseconds(62);
	r = !direct_read();
	interrupts();
	
	delayMicroseconds(410); 
	
	return r;
}

//
// Write a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
IMAGE2_TEXT_SECTION
void OneWire::write_bit(uint8_t v)
{

	if (v & 1) {
		noInterrupts();
		digital_change_dir(this->pin, OUTPUT);
		direct_write(0);
		//delayMicroseconds(6);
		direct_write(1);
		interrupts();
		delayMicroseconds(55);
	} else {
		noInterrupts();
		digital_change_dir(this->pin, OUTPUT); 
		direct_write(0);
		delayMicroseconds(52);
		direct_write(1);
		interrupts();
		delayMicroseconds(1);
	}
}

//
// Read a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
uint8_t OneWire::read_bit(void)
{
	uint8_t r;

	noInterrupts();
	digital_change_dir(this->pin,OUTPUT); // 8 us
	direct_write(0);
	digital_change_dir(this->pin,INPUT); // 8 us
	r = direct_read();
	interrupts();
	delayMicroseconds(45);
	return r;
}

//
// Write a byte. The writing code uses the active drivers to raise the
// pin high, if you need power after the write (e.g. DS18S20 in
// parasite power mode) then set 'power' to 1, otherwise the pin will
// go tri-state at the end of the write to avoid heating in a short or
// other mishap.
//
IMAGE2_TEXT_SECTION
void OneWire::write(uint8_t v, uint8_t power) {
    uint8_t bitMask;

    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
		OneWire::write_bit( (bitMask & v)?1:0);
    }
    if ( !power) {
		noInterrupts();
		pinMode(this->pin, INPUT_PULLDN);
		interrupts();
    }
}

void OneWire::write_bytes(const uint8_t *buf, uint16_t count, bool power) {
  for (uint16_t i = 0 ; i < count ; i++)
    write(buf[i]);
  if (!power) {
    noInterrupts();
	pinMode(this->pin, INPUT);
    interrupts();
  }
}

//
// Read a byte
//
uint8_t OneWire::read() {
    uint8_t bitMask;
    uint8_t r = 0;

    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
	if ( OneWire::read_bit()) r |= bitMask;
    }
    return r;
}

void OneWire::read_bytes(uint8_t *buf, uint16_t count) {
  for (uint16_t i = 0 ; i < count ; i++)
    buf[i] = read();
}

//
// Do a ROM select
//
void OneWire::select(const uint8_t rom[8])
{
    uint8_t i;

    write(0x55);           // Choose ROM

    for (i = 0; i < 8; i++) write(rom[i]);
}

//
// Do a ROM skip
//
void OneWire::skip()
{
    write(0xCC);           // Skip ROM
}

//
// You need to use this function to start a search again from the beginning.
// You do not need to do it for the first search, though you could.
//
void OneWire::reset_search()
{
  // reset the search state
  LastDiscrepancy = 0;
  LastDeviceFlag = FALSE;
  LastFamilyDiscrepancy = 0;
  for(int i = 7; ; i--) {
    ROM_NO[i] = 0;
    if ( i == 0) break;
  }
}

// Setup the search to find the device type 'family_code' on the next call
// to search(*newAddr) if it is present.
//
void OneWire::target_search(uint8_t family_code)
{
   // set the search state to find SearchFamily type devices
   ROM_NO[0] = family_code;
   for (uint8_t i = 1; i < 8; i++)
      ROM_NO[i] = 0;
   LastDiscrepancy = 64;
   LastFamilyDiscrepancy = 0;
   LastDeviceFlag = FALSE;
}

//
// Perform a search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// OneWire::address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use OneWire::reset_search() to
// start over.
//
// --- Replaced by the one from the Dallas Semiconductor web site ---
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
uint8_t OneWire::search(uint8_t *newAddr)
{
   uint8_t id_bit_number;
   uint8_t last_zero, rom_byte_number, search_result;
   uint8_t id_bit, cmp_id_bit;

   unsigned char rom_byte_mask, search_direction;

   // initialize for search
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;

   // if the last call was not the last one
   if (!LastDeviceFlag)
   {
      // 1-Wire reset
      if (!reset())
      {
         // reset the search
         LastDiscrepancy = 0;
         LastDeviceFlag = FALSE;
         LastFamilyDiscrepancy = 0;
		 DiagPrintf("%s reset failed \r\n", __FUNCTION__);
         return FALSE;
      }

      // issue the search command
      write(0xF0);

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = read_bit();
         cmp_id_bit = read_bit();

         // check for no devices on 1-wire
         if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
               search_direction = id_bit;  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
              ROM_NO[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            write_bit(search_direction);

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!(id_bit_number < 65))
      {
         // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
         LastDiscrepancy = last_zero;

         // check for last device
         if (LastDiscrepancy == 0)
            LastDeviceFlag = TRUE;

         search_result = TRUE;
      }
   }

   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !ROM_NO[0])
   {
      LastDiscrepancy = 0;
      LastDeviceFlag = FALSE;
      LastFamilyDiscrepancy = 0;
      search_result = FALSE;
   }
   for (int i = 0; i < 8; i++) newAddr[i] = ROM_NO[i];
   return search_result;
}


#if ONEWIRE_CRC
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

#if ONEWIRE_CRC8_TABLE
// This table comes from Dallas sample code where it is freely reusable,
// though Copyright (C) 2000 Dallas Semiconductor Corporation
static const uint8_t PROGMEM dscrc_table[] = {
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//
// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (note: this might better be done without to
// table, it would probably be smaller and certainly fast enough
// compared to all those delayMicrosecond() calls.  But I got
// confused, so I use this table from the examples.)
//
uint8_t OneWire::crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;

	while (len--) {
		crc = pgm_read_byte(dscrc_table + (crc ^ *addr++));
	}
	return crc;
}
#else
//
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but much smaller, than the lookup table.
//
uint8_t OneWire::crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;
	
	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}
#endif

#if ONEWIRE_CRC16
bool OneWire::check_crc16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc)
{
    crc = ~crc16(input, len, crc);
    return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}

uint16_t OneWire::crc16(const uint8_t* input, uint16_t len, uint16_t crc)
{
    static const uint8_t oddparity[16] =
        { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

    for (uint16_t i = 0 ; i < len ; i++) {
      // Even though we're just copying a byte from the input,
      // we'll be doing 16-bit computation with it.
      uint16_t cdata = input[i];
      cdata = (cdata ^ crc) & 0xff;
      crc >>= 8;

      if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
          crc ^= 0xC001;

      cdata <<= 6;
      crc ^= cdata;
      cdata <<= 1;
      crc ^= cdata;
    }
    return crc;
}
#endif

#endif

