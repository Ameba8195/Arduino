#ifndef _ALEXA_HTTP_SERVICE_H_
#define _ALEXA_HTTP_SERVICE_H_

#include <inttypes.h>

typedef struct {
    char *avs_lwa_redirect_host;
    uint32_t avs_lwa_redirect_host_len;

    char *avs_lwa_api_host;
    uint32_t avs_lwa_api_host_len;

    char *avs_client_id;
    uint32_t avs_client_id_len;

    char *avs_client_secret;
    uint32_t avs_client_secret_len;

    char *avs_redirect_uri;
    uint32_t avs_redirect_uri_len;

    char *avs_product_id;
    uint32_t avs_product_id_len;

    char *avs_product_dsn;
    uint32_t avs_product_dsn_len;

    int (*load_refresh_token_handler)(char *refresh_token, uint32_t *refresh_token_len);
    int (*store_refresh_token_handler)(char *refresh_token, uint32_t refresh_token_len);

    int (*home_page_generator_handler)(char **content, uint32_t *len);
    int (*auth_page_generator_handler)(char **content, uint32_t *len);
} alexa_context_t;

int alexa_get_auth_link(char *link, uint32_t *link_len);
int alexa_is_auth_complete();
void init_alexa_http_service(alexa_context_t *context);

#endif
