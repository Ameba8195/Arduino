#include "Arduino.h"
#include <sockets.h> 
#include <lwip/netif.h>
#include <polarssl/ssl.h>
#include <polarssl/memory.h>
#include <polarssl/net.h>
#include <polarssl/error.h>

#include "ard_ssl.h"

#define SERVER_PORT    443

static unsigned int arc4random(void)
{
	unsigned int res = xTaskGetTickCount();
	static unsigned int seed = 0xDEADB00B;

	seed = ((seed & 0x007F00FF) << 7) ^
		((seed & 0x0F80FF00) >> 8) ^ // be sure to stir those low bits
		(res << 13) ^ (res >> 9);    // using the clock too!

	return seed;
}

static void get_random_bytes(void *buf, size_t len)
{
	unsigned int ranbuf;
	unsigned int *lp;
	int i, count;
	count = len / sizeof(unsigned int);
	lp = (unsigned int *) buf;

	for(i = 0; i < count; i ++) {
		lp[i] = arc4random();  
		len -= sizeof(unsigned int);
	}

	if(len > 0) {
		ranbuf = arc4random();
		memcpy(&lp[i], &ranbuf, len);
	}
}

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	get_random_bytes(output, output_len);
	return 0;
}

int start_ssl_client(sslclient_context *ssl_client, uint32_t ipAddress)
{
	int ret;
	int timeout;
	int port = 443;
	int enable = 1;
	memset(ssl_client, 0, sizeof(sslclient_context));
	ssl_client->socket = -1;
	ssl_client->ssl = (ssl_context *)malloc(sizeof(ssl_context));
	if(ssl_client->ssl == NULL){
		printf("\r\nERROR: malloc ssl context fail\n");
		free(ssl_client);
		free(ssl_client->ssl);
		return -1;
	}
	memset(ssl_client->ssl, 0, sizeof(ssl_context));

	memory_set_own(malloc, free);

	ssl_client->socket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (ssl_client->socket < 0) {
		printf("\r\nERROR opening socket\r\n");
		return -1;
	}
	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ipAddress;
	serv_addr.sin_port = htons(port);

	if(lwip_connect(ssl_client->socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0){
		timeout = 3000;
		lwip_setsockopt(ssl_client->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		lwip_setsockopt(ssl_client->socket, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
	}
	else{
		printf("\r\nConnect to Server failed!\r\n");
		stop_socket(ssl_client->socket);
		return -1;
	}
	
	if((ret = ssl_init(ssl_client->ssl)) != 0) {
		printf("\r\n init ssl failed\r\n");
		net_close(ssl_client->socket);
		ssl_free(ssl_client->ssl);
		free(ssl_client->ssl);
		return -1;
	}

	ssl_set_endpoint(ssl_client->ssl, SSL_IS_CLIENT);
	ssl_set_authmode(ssl_client->ssl, SSL_VERIFY_NONE);
	ssl_set_rng(ssl_client->ssl, my_random, NULL);
	ssl_set_bio(ssl_client->ssl, net_recv, &ssl_client->socket, net_send, &ssl_client->socket);
	if((ret = ssl_handshake(ssl_client->ssl)) != 0) {
		printf("ERROR: ssl_handshake ret(-0x%x)", -ret);
		net_close(ssl_client->socket);
		ssl_free(ssl_client->ssl);
		free(ssl_client->ssl);
		return -1;
	}
	
exit:
	return ssl_client->socket;
}

void stop_ssl_socket(sslclient_context *ssl_client)
{
	net_close(ssl_client->socket);
	ssl_free(ssl_client->ssl);
	free(ssl_client->ssl);
}

int send_ssl_data(sslclient_context *ssl_client, const uint8_t *data, uint16_t len)
{
    int ret;

    ret = ssl_write(ssl_client->ssl, data, len);

    return ret;
}

int get_ssl_receive(sslclient_context *ssl_client, uint8_t* data, int length)
{
    int ret = 0;

	memset(data, 0, length);
    ret = ssl_read(ssl_client->ssl, data, length);

    return ret;
}

sslclient_context *init_ssl_client(void){
	sslclient_context *sslclient = (sslclient_context *)malloc(sizeof(sslclient_context));	
	if(sslclient == NULL){
		printf("\r\nERROR: Malloc(%d bytes) failed\n", sizeof(sslclient_context));
		free(sslclient);
		return NULL;
	}
	memset(sslclient, 0, sizeof(sslclient_context));
	sslclient->socket = -1;
	sslclient->ssl = NULL;
	
	return sslclient;
}
