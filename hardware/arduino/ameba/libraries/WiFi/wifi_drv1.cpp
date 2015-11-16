/*
  wifi_drv.cpp - Library for Arduino Wifi shield.
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "variant.h"

#include "Arduino.h"


#include "lwip/err.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "rt_os_service.h"

#include "wifi_intfs.h"
#include "wl_definitions.h"

#include "wifi_drv1.h"


// scan
static char _networkSsid[WL_NETWORKS_LIST_MAXNUM][WL_SSID_MAX_LENGTH];
static int32_t _networkRssi[WL_NETWORKS_LIST_MAXNUM];
static uint8_t _networkEncr[WL_NETWORKS_LIST_MAXNUM];

//
static char 	_ssid[WL_SSID_MAX_LENGTH];
static uint8_t	_bssid[WL_MAC_ADDR_LENGTH];
static uint8_t _mac[WL_MAC_ADDR_LENGTH];
static uint8_t	_localIp[WL_IPV4_LENGTH];
static uint8_t	_subnetMask[WL_IPV4_LENGTH];
static uint8_t	_gatewayIp[WL_IPV4_LENGTH];


/* TCP/IP and Network Interface Initialisation */
static char gateway[17] = "\0";
static char networkmask[17] = "\0";
static bool use_dhcp = false;
static bool if_enabled = false;

//static Semaphore tcpip_inited(0);



//static Semaphore netif_linked(0);
//static Semaphore netif_up(0);




// Private Methods

//
void WiFiDrv1::getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip)
{
	wl_wifi_get_network_data(ip, mask, gwip);
}



// Public Methods


void WiFiDrv1::wifiDriverInit()
{
	use_dhcp = true;
	wl_wifi_init(&padapter, &pnetif);
}

int8_t WiFiDrv1::disconnect()
{
	wl_wifi_disconnect(padapter);
    return WL_SUCCESS;
}



int8_t WiFiDrv1::startScanNetworks()
{
	Serial.println("WiFiDrv1.startScanNetworks()");
	wl_wifi_scan(padapter);
	return WL_SUCCESS;
}


uint8_t WiFiDrv1::getScanNetworks()
{
	uint8_t num;
	num = wl_wifi_get_scan_networks(padapter, _networkSsid, _networkRssi, _networkEncr, WL_NETWORKS_LIST_MAXNUM);
	return num;

}

int8_t WiFiDrv1::wifiSetPassphrase(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t passphrase_len)
{
	wl_err_t ret = WL_SUCCESS;

	if ( passphrase == NULL || passphrase_len == 0 ) 
		ret = wl_wifi_connect(padapter, ssid, ssid_len,RTW_SECURITY_OPEN, NULL, 0);
	else
		ret = wl_wifi_connect(padapter, ssid, ssid_len,RTW_SECURITY_WPA2_AES_PSK, passphrase, passphrase_len);
	
	if ( ret == WL_SUCCESS ) 
		if_enabled = true;
    return ret;
}

uint8_t* WiFiDrv1::getMacAddress()
{
	wl_wifi_get_mac_address(padapter, (uint8_t*)_mac);

	return _mac;
}

void WiFiDrv1::getIpAddress(IPAddress& ip)
{
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	ip = _localIp;
}

 void WiFiDrv1::getSubnetMask(IPAddress& mask)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	mask = _subnetMask;
 }

 void WiFiDrv1::getGatewayIP(IPAddress& ip)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	ip = _gatewayIp;
 }

char* WiFiDrv1::getCurrentSSID()
{
	wl_wifi_get_current_ssid(padapter, _ssid);
    return _ssid;
}

uint8_t* WiFiDrv1::getCurrentBSSID()
{
	wl_wifi_get_current_bssid(padapter, _bssid);
    return _bssid;
}

int32_t WiFiDrv1::getCurrentRSSI()
{
	return wl_wifi_get_current_rssi(padapter);
}

uint8_t WiFiDrv1::getCurrentEncryptionType()
{

	return wl_wifi_get_current_encType(padapter);
}


char* WiFiDrv1::getSSIDNetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	return _networkSsid[networkItem];
}

int32_t WiFiDrv1::getRSSINetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	return _networkRssi[networkItem];
}


uint8_t WiFiDrv1::getEncTypeNetowrks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	return _networkEncr[networkItem];
}

uint8_t WiFiDrv1::getConnectionStatus()
{

    return wl_wifi_get_connection_status(padapter); 
}



int WiFiDrv1::getHostByName(const char* aHostname, IPAddress& aResult)
{
	uint32_t  _ipAddr;
	IPAddress dummy(0xFF,0xFF,0xFF,0xFF);
	int result = 0;


	result = wl_wifi_get_host_by_name(padapter, aHostname, &_ipAddr);
	if ( result == WL_SUCCESS )
		aResult = _ipAddr;
	return result;
}


WiFiDrv1 wiFiDrv1;

