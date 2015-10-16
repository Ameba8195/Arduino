/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#ifndef __WIFI_INTFS_H_
#define __WIFI_INTFS_H_


#include "wifi_opts.h"


#ifdef __cplusplus
extern "C" {
#endif


#include "rtw_skbuff.h"

typedef enum {
    WL_FAILURE = -1,
    WL_SUCCESS = 1,
    WL_INVALID_LENGTH,
    WL_NOT_SUPPORTED,
    WL_RESOURCES,
    WL_BUSY,
    WL_RETRY, 
    WL_INVALID_ARGS,
    WL_CARD_FAILURE,        
	WL_NO_AP,
	WL_WRONG_SECURITY
} wl_err_t;


//
#define WEP_ENABLED        0x0001
#define TKIP_ENABLED       0x0002
#define AES_ENABLED        0x0004
#define WSEC_SWFLAG        0x0008

#define SHARED_ENABLED  0x00008000
#define WPA_SECURITY    0x00200000
#define WPA2_SECURITY   0x00400000
#define WPS_ENABLED     0x10000000

typedef enum {
    RTW_SECURITY_OPEN           = 0,                                                /**< Open security                           */
    RTW_SECURITY_WEP_PSK        = WEP_ENABLED,                                      /**< WEP Security with open authentication   */
    RTW_SECURITY_WEP_SHARED     = ( WEP_ENABLED | SHARED_ENABLED ),                 /**< WEP Security with shared authentication */
    RTW_SECURITY_WPA_TKIP_PSK   = ( WPA_SECURITY  | TKIP_ENABLED ),                 /**< WPA Security with TKIP                  */
    RTW_SECURITY_WPA_AES_PSK    = ( WPA_SECURITY  | AES_ENABLED ),                  /**< WPA Security with AES                   */
    RTW_SECURITY_WPA2_AES_PSK   = ( WPA2_SECURITY | AES_ENABLED ),                  /**< WPA2 Security with AES                  */
    RTW_SECURITY_WPA2_TKIP_PSK  = ( WPA2_SECURITY | TKIP_ENABLED ),                 /**< WPA2 Security with TKIP                 */
    RTW_SECURITY_WPA2_MIXED_PSK = ( WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED ),   /**< WPA2 Security with AES & TKIP           */

    RTW_SECURITY_WPS_OPEN       = WPS_ENABLED,                                      /**< WPS with open security                  */
    RTW_SECURITY_WPS_SECURE     = (WPS_ENABLED | AES_ENABLED),                      /**< WPS with AES security                   */

    RTW_SECURITY_UNKNOWN        = -1,                                               /**< May be returned by scan function if security is unknown. Do not pass this to the join function! */

    RTW_SECURITY_FORCE_32_BIT   = 0x7fffffff                                        /**< Exists only to force rtw_security_t type to 32 bits */
} rtw_security_t;


extern void wl_wifi_init(void **ppadapter, void**ppnetif);
extern void wl_wifi_scan(void *padapter);
extern wl_err_t wl_wifi_connect(
	void *padapter,
	char*			ssid,
	int				ssid_len,
	rtw_security_t 	security_type, 
	const char*		password,
	const int		password_len);

void wl_wifi_disconnect(
	void* padapter1);

extern void wl_wifi_get_mac_address(void* padapter1, u8* mac_addr);

//GTK
sint wl_set_key(void* padapter1, void* psecuritypriv1, sint keyid, u8 set_tx);


//PTK
extern u8 wl_set_stakey(void* padapter1, u8 *psta, u8 unicast_key);


// TX

extern int8_t wl_netif_tx(struct sk_buff** skb);
extern void wl_netif_rx(void* skb1);

// SCAN queue
extern int wl_wifi_get_scan_networks(void* padapter1, char ssid_list[][32], int32_t rssi_list[], uint8_t encr_list[], int max_list_num );

extern int wl_wifi_get_connection_status(void* padapter1);

extern void wl_wifi_get_network_data(uint8_t *ip, uint8_t *mask, uint8_t *gw_ip);


// connected AP status

extern void wl_wifi_get_current_ssid(void* padapter1, char* ssid);

extern void wl_wifi_get_current_bssid(void* padapter1, u8* bssid);

extern long wl_wifi_get_current_rssi(void* padapter1);

extern uint8_t wl_wifi_get_current_encType(void* padapter1);

extern wl_err_t wl_wifi_get_host_by_name(void* padapter1, const char *aHostName, uint32_t *paddr);

extern void wl_wifi_indicate_connect(void* padapter);
extern void wl_wifi_indicate_disconnect(void* padapter);

extern void wl_set_scan_lifetime(void* padapter1, u8 lifetime);

#ifdef __cplusplus
}
#endif



#endif	//__wifi_INTFS_H_

