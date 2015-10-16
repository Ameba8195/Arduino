/*
 * TwoWire.h - TWI/I2C library for Arduino Due
 * Copyright (c) 2011 Cristian Maglie <c.maglie@bug.st>.
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "arduino.h"

extern "C" {
#include <string.h>
}


#include "Wire.h"



extern "C" {
extern
_LONG_CALL_ u32
DiagPrintf(const char *fmt, ...);

//i2c
i2c_t i2cmaster_wire1;

}

TwoWire::TwoWire(PinName SDA_Pin, PinName SCL_Pin)
{
	DiagPrintf("TwoWire initializing \r\n");
	
	setpin(SDA_Pin, SCL_Pin);
	setI2CMaster(&i2cmaster_wire1);
}

void TwoWire::setpin(PinName SDA_Pin, PinName SCL_Pin )
{
	this->SDA_pin = SDA_Pin;
	this->SCL_pin = SCL_Pin;
}

void TwoWire::setI2CMaster(i2c_t *pObj )
{
	this->pI2Cmaster = pObj;
}

i2c_t * TwoWire::getI2CMaster(void)
{
	return this->pI2Cmaster;
}

void TwoWire::begin(void) {
	this->rxBufferIndex = 0;
	this->rxBufferLength = 0;
	this->txAddress = 0;
	this->txBufferLength = 0;
	this->srvBufferIndex = 0; 
	this->srvBufferLength = 0;
	this->status = UNINITIALIZED;
	this->twiClock = this->TWI_CLOCK;

	
	i2c_init(this->pI2Cmaster, (PinName)this->SDA_pin, (PinName)this->SCL_pin); 	
    i2c_frequency(this->pI2Cmaster,this->twiClock);
	
	status = MASTER_IDLE;
}

void TwoWire::setClock(uint32_t frequency) {
	this->twiClock = frequency;
    i2c_frequency(this->pI2Cmaster,this->twiClock);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
	int readed=0;
	
	if (quantity > BUFFER_LENGTH)
		quantity = BUFFER_LENGTH;

	// perform blocking read into buffer
	readed = i2c_read(this->pI2Cmaster, (int)address, (char*)&this->rxBuffer[0], (int)quantity, (int)sendStop);

	// i2c_read error;
	if ( readed != quantity ) {
		DiagPrintf("requestFrom: readed=%d, quantity=%d : ERROR\n", readed, quantity);
		//i2c_reset(this->pI2Cmaster);
		//this->begin();
		return readed;
	}
	
	// set rx buffer iterator vars
	this->rxBufferIndex = 0;
	this->rxBufferLength = readed;

	return readed;
}


uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity) {
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity) {
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop) {
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) sendStop);
}

void TwoWire::beginTransmission(uint8_t address) {
	this->status = MASTER_SEND;

	// save address of target and empty buffer
	this->txAddress = address;
	this->txBufferLength = 0;
}

void TwoWire::beginTransmission(int address) {
	beginTransmission((uint8_t) address);
}

size_t TwoWire::write(uint8_t data) {
	if (this->status == MASTER_SEND) {
		if (this->txBufferLength >= BUFFER_LENGTH)
			return 0;
		this->txBuffer[this->txBufferLength++] = data;
	} else {
		if (this->srvBufferLength >= BUFFER_LENGTH)
			return 0;
		this->srvBuffer[srvBufferLength++] = data;
	}
	return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity) {
	if (this->status == MASTER_SEND) {
		for (size_t i = 0; i < quantity; ++i) {
			if (this->txBufferLength >= BUFFER_LENGTH)
				return i;
			this->txBuffer[this->txBufferLength++] = data[i];
		}
	} else {
		for (size_t i = 0; i < quantity; ++i) {
			if (this->srvBufferLength >= BUFFER_LENGTH)
				return i;
			this->srvBuffer[this->srvBufferLength++] = data[i];
		}
	}
	return quantity;
}

int TwoWire::available(void) {
	return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void) {
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex++];
	return -1;
}


int TwoWire::peek(void) {
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex];
	return -1;
}

void TwoWire::flush(void) {
	// Do nothing, use endTransmission(..) to force
	// data transfer.
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
	return endTransmission(true);
}



uint8_t TwoWire::endTransmission(uint8_t sendStop) {
	uint8_t error = 0;
	
    error = i2c_write(this->pI2Cmaster, (int)this->txAddress, (const char*)&this->txBuffer[0], (int)this->txBufferLength, (int)sendStop);

	txBufferLength = 0;		// empty buffer
	status = MASTER_IDLE;
	return error;
}


TwoWire Wire = TwoWire(PD_7, PD_6);



