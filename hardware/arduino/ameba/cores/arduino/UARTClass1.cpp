/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UARTClass1.h"


extern "C"{

#include "hal_irqn.h"
#include "hal_diag.h"
#include "hal_platform.h"

} // extern C


// Constructors ////////////////////////////////////////////////////////////////
//  when pmc / nvic is ready 

#define UART1_TX    PA_7   
#define UART1_RX    PA_6   

static RingBuffer rx_buffer1;

UARTClass1 Serial1(&rx_buffer1);

extern "C" {



void uart_irq(uint32_t id, SerialIrq event)
{
	Serial1.IrqHandler(event);
}


} // extern C


// Public Methods //////////////////////////////////////////////////////////////
UARTClass1::UARTClass1(RingBuffer* pRx_buffer )
{
	_rx_buffer = pRx_buffer ;
}


void UARTClass1::IrqHandler(SerialIrq event)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(this->sobj.hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;
	
    uint8_t     data = 0;

	if ( event == RxIrq ) { 
	    data = serial_getc(&(this->sobj));
		_rx_buffer->store_char(data);
	} 
}


void UARTClass1::begin( const uint32_t dwBaudRate )
{
	
	serial_init(&(this->sobj),UART1_TX,UART1_RX);

	serial_baud(&(this->sobj),dwBaudRate);
	serial_format(&(this->sobj), 8, ParityNone, 1);
	serial_set_pullNone();

    serial_irq_handler(&(this->sobj), uart_irq, (uint32_t)&(this->sobj));
    serial_irq_set(&(this->sobj), RxIrq, 1);
    serial_irq_set(&(this->sobj), TxIrq, 1);
}

void UARTClass1::set_baud( const uint32_t dwBaudRate )
{
	serial_baud(&(this->sobj),dwBaudRate);
}

void UARTClass1::end( void )
{
	// clear any received data
	_rx_buffer->_iHead = _rx_buffer->_iTail ;
}

int UARTClass1::available( void )
{
	return (uint32_t)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
}

int UARTClass1::peek( void )
{

  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
    return -1 ;

  return _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;

}


int UARTClass1::read( void )
{
	// if the head isn't ahead of the tail, we don't have any characters
	if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
	  return -1 ;
	
	uint8_t uc = _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;
	_rx_buffer->_iTail = (unsigned int)(_rx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;
	return uc ;	
}

void UARTClass1::flush( void )
{

  while ( serial_writable(&(this->sobj)) != 1 );
}

size_t UARTClass1::write( const uint8_t uc_data )
{
	serial_putc(&(this->sobj), uc_data);
  	return 1;
}


