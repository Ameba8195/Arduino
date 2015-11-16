/*
  WiFiServer.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

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

#include <string.h>

#include "WiFi1.h"
#include "WiFiClient1.h"
#include "WiFiServer1.h"

WiFiServer1::WiFiServer1(uint16_t port)
{
    _port = port;
}

void WiFiServer1::begin()
{
	DiagPrintf("WiFiServer1 begin(), port = %d \r\n", _port);
	this->_server.bind(_port);
	this->_server.listen();
	
}

WiFiClient1 WiFiServer1::available(byte* status)
{
	this->_server.accept(this->tcpSock);
	//this->tcpSock.set_blocking(false, 15000);
    return WiFiClient1(&(this->tcpSock));
}



size_t WiFiServer1::write(uint8_t b)
{
    return write(&b, 1);
}

size_t WiFiServer1::write(const uint8_t *buffer, size_t size)
{
	size_t n = 0;
	this->tcpSock.send_all((const char*)buffer,size);
    return size;
}

