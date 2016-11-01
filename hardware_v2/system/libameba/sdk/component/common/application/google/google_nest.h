#ifndef GOOGLENEST_H
#define GOOGLENEST_H

#include <polarssl/ssl.h>

typedef struct {
	int socket;
	char *host;
	ssl_context ssl;
} googlenest_context;

#define BUFFER_SIZE       512


int gn_connect(googlenest_context *googlenest, char *host, int port);
void gn_close(googlenest_context *googlenest);
int gn_put(googlenest_context *googlenest, char *uri, char *content);
int gn_patch(googlenest_context *googlenest, char *uri, char *content);
int gn_post(googlenest_context *googlenest, char *uri, char *content, unsigned char *out_buffer, size_t out_len);
int gn_get(googlenest_context *googlenest, char *uri, unsigned char *out_buffer, size_t out_len);
int gn_delete(googlenest_context *googlenest, char *uri);
int gn_stream(googlenest_context *googlenest, char *uri);
void google_retrieve_data_hook_callback(void (*callback)(char *));

#endif

