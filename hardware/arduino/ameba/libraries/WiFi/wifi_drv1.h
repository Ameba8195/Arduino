/*
  wifi_drv.h - Library for Arduino Wifi shield.
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

#ifndef WiFi_Drv1_h
#define WiFi_Drv1_h

#include <inttypes.h>

#include "lwip/err.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"

#include "wifi_intfs.h"
#include "wl_definitions.h"

#include "IPAddress.h"

class WiFiDrv1
{
protected: 
	
	void getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip);
		
public:


	void* padapter;
	void* pnetif;

    /*
     * Driver initialization
     */
    void wifiDriverInit();


    /*
     * Start scan WiFi networks available
     *
     * return: Number of discovered networks
     */
    int8_t startScanNetworks();

    /*
     * Get the networks available
     *
     * return: Number of discovered networks
     */
    uint8_t getScanNetworks();

	char* getSSIDNetoworks(uint8_t networkItem);
	int32_t getRSSINetoworks(uint8_t networkItem);
	
	uint8_t getEncTypeNetowrks(uint8_t networkItem);

    /* Start Wifi connection with passphrase
     * the most secure supported mode will be automatically selected
     *
     * param ssid: Pointer to the SSID string.
     * param ssid_len: Lenght of ssid string.
     * param passphrase: Passphrase. Valid characters in a passphrase
     *        must be between ASCII 32-126 (decimal).
     * param len: Lenght of passphrase string.
     * return: WL_SUCCESS or WL_FAILURE
     */
   int8_t wifiSetPassphrase(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len);

   /*
	* Disconnect from the network
	*
	* return: WL_SUCCESS or WL_FAILURE
	*/
   int8_t disconnect();

   uint8_t* getMacAddress();

   /*
	* Get the interface IP address.
	*
	* return: copy the ip address value in IPAddress object
	*/
   void getIpAddress(IPAddress& ip);
   
   /*
	* Get the interface subnet mask address.
	*
	* return: copy the subnet mask address value in IPAddress object
	*/
   void getSubnetMask(IPAddress& mask);
   
   /*
	* Get the gateway ip address.
	*
	* return: copy the gateway ip address value in IPAddress object
	*/
   void getGatewayIP(IPAddress& ip);

   /*
	* Return the current SSID associated with the network
	*
	* return: ssid string
	*/
   char* getCurrentSSID();
   
   /*
	* Return the current BSSID associated with the network.
	* It is the MAC address of the Access Point
	*
	* return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
	*/
   uint8_t* getCurrentBSSID();

   /*
	* Return the current RSSI /Received Signal Strength in dBm)
	* associated with the network
	*
	* return: signed value
	*/
   int32_t getCurrentRSSI();

   /*
	* Return the Encryption Type associated with the network
	*
	* return: one value of wl_enc_type enum
	*/
   uint8_t getCurrentEncryptionType();
   
   
   uint8_t getConnectionStatus();

   
   /*
	* Return Connection status.
	*
	* return: one of the value defined in wl_status_t
	*/
   uint8_t status();

   
   int getHostByName(const char* aHostname, IPAddress& aResult);

};

extern WiFiDrv1 wiFiDrv1;

#endif
