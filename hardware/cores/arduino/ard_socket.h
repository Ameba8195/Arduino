#ifndef ARD_SOCKET_H
#define ARD_SOCKET_H
#include "main.h"
	int start_server(uint16_t port, uint8_t sock, uint8_t protMode);

	int tcp_listen(uint8_t sock, int max);

	uint8_t get_available(uint8_t sock);

	int get_receive(uint8_t sock, uint8_t* data, int length);

	void stop_socket(uint8_t sock);

	int send_data(uint8_t sock, const uint8_t *data, uint16_t len);

	int  start_client(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode);

#endif
