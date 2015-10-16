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
#include "UARTClass.h"


extern "C"{

#include "hal_irqn.h"
#include "hal_diag.h"


} // extern C


UARTClass::UARTClass(IRQn_Type dwIrq, RingBuffer* pRx_buffer )
{

	_rx_buffer = pRx_buffer ;

	_dwIrq=dwIrq ;
}


// Protected Methods //////////////////////////////////////////////////////////////




// Public Methods //////////////////////////////////////////////////////////////


void UARTClass::IrqHandler( void )
{

    uint8_t     data = 0;
    BOOL    	PullMode = _FALSE;
    uint32_t 	IrqEn = DiagGetIsrEnReg();

    DiagSetIsrEnReg(0);

    data = DiagGetChar(PullMode);
	if ( data > 0 ) 
		_rx_buffer->store_char(data);

    DiagSetIsrEnReg(IrqEn);

}


void UARTClass::begin( const uint32_t dwBaudRate )
{
	//TODO : loguart set baud
}

void UARTClass::end( void )
{

  // clear any received data
  _rx_buffer->_iHead = _rx_buffer->_iTail ;

}

int UARTClass::available( void )
{
  return (uint32_t)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
}

int UARTClass::peek( void )
{

  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
    return -1 ;

  return _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;

}

int UARTClass::read( void )
{
  // if the head isn't ahead of the tail, we don't have any characters
  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
    return -1 ;

  uint8_t uc = _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;
  _rx_buffer->_iTail = (unsigned int)(_rx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;
  return uc ;

}

void UARTClass::flush( void )
{
// TODO: 
// while ( serial_writable(&(this->sobj)) != 1 );
/*
  // Wait for transmission to complete
  while ((_pUart->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY)
    ;
*/
}

size_t UARTClass::write( const uint8_t uc_data )
{

	HalSerialPutcRtl8195a(uc_data);
  	return 1;

}


