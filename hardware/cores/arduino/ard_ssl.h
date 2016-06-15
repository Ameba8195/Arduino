#ifndef ARD_SSL_H
#define ARD_SSL_H

struct ssl_context;

typedef struct {
	int socket;
	ssl_context *ssl;
} sslclient_context;

int start_ssl_client(sslclient_context *ssl_client, uint32_t ipAddress);

void stop_ssl_socket(sslclient_context *ssl_client);

int send_ssl_data(sslclient_context *ssl_client, const uint8_t *data, uint16_t len);

int get_ssl_receive(sslclient_context *ssl_client, uint8_t* data, int length);

sslclient_context *init_ssl_client(void);

#endif
