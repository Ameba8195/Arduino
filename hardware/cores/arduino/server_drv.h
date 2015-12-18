/*
  server_drv.h - Library for Arduino Wifi shield.
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

#ifndef Server_Drv_h
#define Server_Drv_h

#include <inttypes.h>

typedef enum eProtMode {TCP_MODE, UDP_MODE}tProtMode;
#define	DATA_LENTH		128
class ServerDrv
{
public:
	void startServer(uint16_t port, uint8_t sock, uint8_t protMode=TCP_MODE);
	uint8_t getAvailable(uint8_t sock);
	static int startClient(char* ipAddress, uint16_t port, uint8_t sock, uint8_t protMode=TCP_MODE);
	void stopClient(uint8_t sock);
	bool getData(uint8_t sock, uint8_t *data, uint8_t peek = 0);
	int getDataBuf(uint8_t sock, uint8_t *_data, uint16_t _dataLen);
	bool sendData(uint8_t sock, const uint8_t *data, uint16_t len);
	uint16_t availData(uint8_t sock);
	
private:    	
	uint8_t _readchar[1];	
	bool _readchar_set;
};

extern ServerDrv serverDrv;

#endif
