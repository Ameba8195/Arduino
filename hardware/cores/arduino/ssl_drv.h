#ifndef SSL_DRV_H
#define SSL_DRV_H
#include <inttypes.h>

#define	DATA_LENTH		128
#ifdef __cplusplus
extern "C" {
#include "ard_ssl.h"
}
#endif

class SSLDrv
{
public:
	int startClient(sslclient_context *ssl_client, uint32_t ipAddress);
	void stopClient(sslclient_context *ssl_client);
	bool getData(sslclient_context *ssl_client, uint8_t *data);
	int getDataBuf(sslclient_context *ssl_client, uint8_t *_data, uint16_t _dataLen);
	bool sendData(sslclient_context *ssl_client, const uint8_t *data, uint16_t len);
	uint16_t availData(sslclient_context *ssl_client);
	sslclient_context *init(void);

private:    	
	bool _available;
};

#endif
