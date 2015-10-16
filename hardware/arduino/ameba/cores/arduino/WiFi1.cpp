/*
  WiFi.cpp - Library for Arduino Wifi shield.
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

#include "variant.h"

#include "WiFi1.h"
#include "wifi_drv1.h"
#include "wl_definitions.h"


int16_t 	WiFiClass1::_state[MAX_SOCK_NUM] = { NA_STATE, NA_STATE, NA_STATE, NA_STATE };


WiFiClass1::WiFiClass1()
{
}

void WiFiClass1::init()
{
	wiFiDrv1.wifiDriverInit();
}



int WiFiClass1::begin(char* ssid)
{
	this->begin(ssid, NULL);
}

int WiFiClass1::begin(char* ssid, const char *passphrase)
{
	int8_t ret;

	if (passphrase == NULL )
		ret =  wiFiDrv1.wifiSetPassphrase(ssid, strlen(ssid), NULL, 0);
	else
		ret =  wiFiDrv1.wifiSetPassphrase(ssid, strlen(ssid), passphrase, strlen(passphrase));
	
	if ( ret == WL_SUCCESS ) {
		ret = this->status();
	}
	return ret;


}

int WiFiClass1::disconnect()
{
    return wiFiDrv1.disconnect();
}

uint8_t* WiFiClass1::macAddress(uint8_t* mac)
{
	uint8_t* _mac = wiFiDrv1.getMacAddress();
	for (int i=0; i<WL_MAC_ADDR_LENGTH; i++)
		mac[i] = _mac[WL_MAC_ADDR_LENGTH-i-1];
    return mac;
}

IPAddress WiFiClass1::localIP()
{
	IPAddress ret;
	wiFiDrv1.getIpAddress(ret);
	return ret;
}

IPAddress WiFiClass1::subnetMask()
{
	IPAddress ret;
	wiFiDrv1.getSubnetMask(ret);
	return ret;
}

IPAddress WiFiClass1::gatewayIP()
{
	IPAddress ret;
	wiFiDrv1.getGatewayIP(ret);
	return ret;
}

char* WiFiClass1::SSID()
{
    return wiFiDrv1.getCurrentSSID();
}

uint8_t* WiFiClass1::BSSID(uint8_t* bssid)
{
	uint8_t* _bssid = wiFiDrv1.getCurrentBSSID();
	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFiClass1::RSSI()
{
    return wiFiDrv1.getCurrentRSSI();
}

uint8_t WiFiClass1::encryptionType()
{
    return wiFiDrv1.getCurrentEncryptionType();
}

int8_t WiFiClass1::scanNetworks()
{
	uint8_t attempts = 10;
	uint8_t numOfNetworks = 0;

	if ( wiFiDrv1.startScanNetworks() == WL_FAILURE)
		return WL_FAILURE;
 	numOfNetworks = wiFiDrv1.getScanNetworks();
	return numOfNetworks;
}


char* WiFiClass1::SSID(uint8_t networkItem)
{
	return wiFiDrv1.getSSIDNetoworks(networkItem);
}

int32_t WiFiClass1::RSSI(uint8_t networkItem)
{
	return wiFiDrv1.getRSSINetoworks(networkItem);
}


uint8_t WiFiClass1::encryptionType(uint8_t networkItem)
{
    return wiFiDrv1.getEncTypeNetowrks(networkItem);
}


uint8_t WiFiClass1::status()
{
    return wiFiDrv1.getConnectionStatus();
}

int WiFiClass1::hostByName(const char* aHostname, IPAddress& aResult)
{
	return wiFiDrv1.getHostByName(aHostname, aResult);
}

WiFiClass1 WiFi1;

