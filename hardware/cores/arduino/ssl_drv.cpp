#include "ssl_drv.h"
#ifdef __cplusplus
extern "C" {
#include "platform_stdlib.h"
}
#endif

uint16_t SSLDrv::availData(sslclient_context *ssl_client)
{
	int ret;

	if (ssl_client->socket < 0)		
		return 0;
	
	if(_available) {
		return 1;
	} else {
		ret = get_ssl_receive(ssl_client, c, 1);
		if ( ret == 1 ) {
			_available = true;
			read_c = true;
			return 1;	
		} 	
		else{
			return 0;	
		}
	}
}

bool SSLDrv::getData(sslclient_context *ssl_client, uint8_t *data)
{
	int ret = 0;

	if(read_c){
		memcpy(data, c, 1);
		read_c = false;
		return true;
	}
	
	ret = get_ssl_receive(ssl_client, data, 1);
	
	if (ret == 1) {
		return true;
	}
 
	return false;
}

int SSLDrv::getDataBuf(sslclient_context *ssl_client, uint8_t *_data, uint16_t _dataLen)
{
	int ret;

    _available = false;

	ret = get_ssl_receive(ssl_client, _data, _dataLen);

	return ret;
}

void SSLDrv::stopClient(sslclient_context *ssl_client)
{
	stop_ssl_socket(ssl_client);
    _available = false;
}

bool SSLDrv::sendData(sslclient_context *ssl_client, const uint8_t *data, uint16_t len)
{
    int ret;

    if (ssl_client->socket < 0)
        return false;        	

    ret = send_ssl_data(ssl_client, data, len);

    if (ret == 0) {  
        return false;
    }

    return true;
}

int SSLDrv::startClient(sslclient_context *ssl_client, uint32_t ipAddress, unsigned char* rootCABuff, unsigned char* cli_cert, unsigned char* cli_key)
{
    int ret;

    ret = start_ssl_client(ssl_client, ipAddress, rootCABuff, cli_cert, cli_key);

    return ret;
}

sslclient_context *SSLDrv::init(void)
{
	_available = false;
	read_c = false;
	return init_ssl_client();
}
