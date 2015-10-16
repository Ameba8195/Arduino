/*
  WiFiClient.cpp - Library for Arduino Wifi shield.
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

#include "wl_definitions.h"
#include "WiFi1.h"
#include "WiFiClient1.h"



WiFiClient1::WiFiClient1()  : _sock(MAX_SOCK_NUM){
	_pTcpSocket = new TCPSocketConnection();
}

WiFiClient1::WiFiClient1(TCPSocketConnection* s)  
{	
	_pTcpSocket = s;
	_sock = _pTcpSocket->get_socket_fd();
	WiFiClass1::_state[_sock] = _sock;
	_readchar_set = false;
}



int WiFiClient1::connect(const char* host, uint16_t port) {
	IPAddress remote_addr;
	if (WiFi1.hostByName(host, remote_addr))
	{
		return connect(remote_addr, port);
	}
	return 0;
}

int WiFiClient1::connect(IPAddress ip, uint16_t port) 
{
	int ret;

    _sock = getFirstSocket();
    if (_sock != SOCK_NOT_AVAIL)
    {
		ret = _pTcpSocket->connect(ip.get_address(), port);
		if ( ret != 0 ) {
			Serial.print("WiFiClient1 : connect failed ret=");
			Serial.println(ret);
			return 0; // socket connect failed. 
		}

		
    	if (!connected())
       	{
			Serial.println("WiFiClient1 : connect failed : connected is not true");
    		return 0;
    	}

		_sock = _pTcpSocket->get_socket_fd();
		WiFiClass1::_state[_sock] = _sock;
		_readchar_set = false;
	
		
    }
	else{
    
    	Serial.println("No Socket available");
    	return 0;
    }
    return 1;
}

int WiFiClient1::available() {
	int ret;

	if (_sock == 255) return 0;
	if (_pTcpSocket->is_connected() == false ) return 0;

	ret = _pTcpSocket->receive(&_readchar,1);
	if ( ret == 1 ) {
		_readchar_set = true;
		return 1;
	} else {
		return 0;
	}
}

size_t WiFiClient1::write(uint8_t b) {
	  return write(&b, 1);
}

size_t WiFiClient1::write(const uint8_t *buf, size_t size) {

  return _pTcpSocket->send_all((char*)buf, (int)size);
}


int WiFiClient1::read() {
	uint8_t ch;
	
	if ( _readchar_set ) {
		ch = (uint8_t)_readchar;
		_readchar_set = false;
	} else {
		read(&ch, 1);
	}
	
	return (int)ch ;
}


int WiFiClient1::read(uint8_t* buf, size_t size) {
  int _size;
  int ret;
  int n;

  _size = size;
  n = 0;
  
  if ( _readchar_set ) {
  	buf[0] = _readchar;
	_readchar_set = false;
	buf = buf+1;
	_size = _size - 1;
	n = 1;
	if ( _size ==0 ) return n;
  }

  ret = _pTcpSocket->receive((char*)buf, (int)_size);
  if ( ret < 0 ) return ret;
  return (n+ret);
}

int WiFiClient1::peek() {
	uint8_t b;
	if (!available())
		return -1;

	b = (uint8_t)_readchar;
	_readchar_set = false;
	return b;
}

void WiFiClient1::flush() {
  while (available())
    read();
}

void WiFiClient1::stop() {

  if (_sock == 255)
    return;

  _pTcpSocket->close();
  WiFiClass1::_state[_sock] = NA_STATE;

  _sock = 255;
}

uint8_t WiFiClient1::connected() {

  if (_sock == 255) {
    return 0;
  } else {
  	return ( _pTcpSocket->is_connected() == true )? 1 : 0;
  }
}

WiFiClient1::operator bool() {
  if ( _sock == 255 ) return false;
  return _pTcpSocket->is_connected();
}

// Private Methods
uint8_t WiFiClient1::getFirstSocket()
{
    for (int i = 0; i < MAX_SOCK_NUM; i++) {
      if (WiFiClass1::_state[i] == NA_STATE)
      {
          return i;
      }
    }
    return SOCK_NOT_AVAIL;
}


char* WiFiClient1::get_address(void)
{
	return _pTcpSocket->get_address();
}

