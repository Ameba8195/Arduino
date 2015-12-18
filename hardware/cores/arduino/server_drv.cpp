#include "server_drv.h"

#ifdef __cplusplus
extern "C" {
#include "ard_socket.h"
#include "platform_stdlib.h"
}
#endif
// Start server TCP on port specified
void ServerDrv::startServer(uint16_t port, uint8_t sock, uint8_t protMode)
{
	start_server(port, sock, protMode);
	if(protMode == TCP_MODE)
		tcp_listen(sock, 1);
}

uint8_t ServerDrv::getAvailable(uint8_t sock)
{
	uint8_t newsock;
	newsock  = get_available(sock);
	if(newsock != -1){
		return newsock;
	}
	else{
		return 255;
	}
}

uint16_t ServerDrv::availData(uint8_t sock)
{
	int ret;
	
	if (sock == 255)		
		return 0;
	
	if(_readchar_set)
		return 1;
	else{
		memset(_readchar, 0, 1);
		ret = get_receive(sock, _readchar, 1);	
		if ( ret == 1 ) {		
			_readchar_set = true;		
			return 1;	
		} 	
		else{
			return 0;	
		}
	}
}

bool ServerDrv::getData(uint8_t sock, uint8_t *data, uint8_t peek)
{
	int ret = 0;
	
	if ( _readchar_set ) {
		memcpy(data, _readchar, 1);
		memset(_readchar, 0, 1);
		_readchar_set = false;	
		return true;
	} 
	else {		
		ret = get_receive(sock, data, 1);
	}
	
	if (ret == 1){
		return true;
	}
 
	return false;
}

int ServerDrv::getDataBuf(uint8_t sock, uint8_t *_data, uint16_t _dataLen)
{
  	uint16_t _size;
	int ret; 
	int n; 
	_size = _dataLen;  
	n = 0;    
	if ( _readchar_set ) {
		_data[0] = _readchar[0];		
		_readchar_set = false;	
		_data = _data+1;		
		_size = _size - 1;	
		n = 1;		
		if ( _size ==0 ) 	
			return true;
	}  	
	ret = get_receive(sock, _data, _size);
	return ret;
}

void ServerDrv::stopClient(uint8_t sock)
{
	stop_socket(sock);
	memset(_readchar, 0, 1);
	_readchar_set = false;	
}

bool ServerDrv::sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
	if (sock < 0)
		return false;        	
	int ret;
	ret = send_data(sock, data, len);
	
	 if (ret == 0) {  
		//_is_connected = false;     
		return false;
	 }
	return true;
}

int ServerDrv::startClient(char* ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
	int ret;
	ret = start_client(ipAddress, port, sock, protMode);

	return ret;
}
ServerDrv serverDrv;
