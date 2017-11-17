#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <../network/ssl/polarssl-1.3.8/include/polarssl/platform.h>
#include <GoogleCloud.h>
#include <google/google_cloud_iot/google_cloud_iot.h>


#ifdef __cplusplus
}
#endif


#ifndef SDRAM_BSS_SECTION
#define SDRAM_BSS_SECTION                        \
        SECTION(".sdram.bss")
#endif

#define send_buf_len	1024
#define read_buf_len	64
#define count   100

GoogleCloud_context gc_client;

GoogleCloudClass::GoogleCloudClass()
{
     _privateKeyBuff = NULL;
}

int GoogleCloudClass::gcConnect(char* project_id, char* registry_id, char* device_id)
{	
	int ret;

	ret = gc_client_init(&gc_client, project_id, registry_id, device_id, send_buf_len, read_buf_len);
	
	if(ret == SUCCESS)
		printf("\r\nThe Google Cloud Client init OK\r\n");
	else{ 
		printf("\r\nInit Google Cloud Client failed\r\n");
		return ret;
	}

	ret = gc_client_connect(&gc_client, _privateKeyBuff);
	
	if(ret == SUCCESS)
		printf("\r\nThe Google Cloud Client is starting\r\n");
	else{ 
		printf("\r\nStart the Google Cloud Client failed\r\n");
		return ret;
	}
  return SUCCESS;
}

int GoogleCloudClass::publish()	
{
	char payload[32];
	int ret;
	
	for(int i = 0; i < count; i++){
		memset(payload, 0, 32);
		sprintf(payload, "This is Ameba2's %d message", i);
		printf("\r\nPublish the payload %s with len: %d\n", payload, strlen(payload));
		ret = gc_publish(&gc_client, payload, strlen(payload), 1);
		
		if(ret == 0)
			printf("\r\nPublish message ok\r\n");
		else{ 
			printf("\r\nPublish message failed\r\n");
			break;
		}
		vTaskDelay(1000);

		if (gc_client.client.isconnected == 0) {
			printf("\r\nMQTT disconnect with server\r\n");
			break;
		}
	}
	return ret;
	
}

bool GoogleCloudClass::isconnected()
{
	return gc_client.client.isconnected;
}

void GoogleCloudClass::setPrivatekey(unsigned char *privateKey) 
{
		_privateKeyBuff = privateKey;
}



GoogleCloudClass gc_class;
