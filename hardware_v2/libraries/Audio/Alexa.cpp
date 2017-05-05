#ifdef __cplusplus
extern "C" {
#endif

#include "alexa/alexa_service.h"

alexa_context_t alexa_context;

#ifdef __cplusplus
}
#endif

#include "Arduino.h"
#include "Alexa.h"

AlexaClass::AlexaClass()
{
    memset(&alexa_context, 0, sizeof(alexa_context_t));
    context = &alexa_context;
    aud_rxbuf = NULL;
    aud_rxbuf_size = 0;
    aud_txbuf = NULL;
    aud_txbuf_size = 0;
    codec_init_handler = NULL;
}

AlexaClass::~AlexaClass()
{
}

void AlexaClass::begin()
{
    alexa_context.load_refresh_token_handler(NULL, NULL);
    init_alexa_audio_service((void (*)())codec_init_handler, aud_rxbuf, aud_rxbuf_size, aud_txbuf, aud_txbuf_size);
    init_alexa_http_service((alexa_context_t *)context);
    init_alexa_mdns_service();
    init_alexa_http2_service();
}

void AlexaClass::setAudioTxBuffering(uint32_t threshold)
{
    alexa_audio_set_tx_buffering_threshold(threshold);
}

int AlexaClass::getAuthLink(char *link, uint32_t *link_len)
{
    alexa_get_auth_link(link, link_len);
}

int AlexaClass::isAuthComplete()
{
    return alexa_is_auth_complete();
}

int AlexaClass::audioRecordState()
{
    return alexa_upload_state();
}

void AlexaClass::audioRecordStart()
{
    if (!alexa_audio_is_recording()) {
        alexa_audio_record_start();
    }
}

void AlexaClass::audioRecordStop()
{
    if (alexa_audio_is_recording()) {
        alexa_audio_record_stop();
    }
}

void AlexaClass::setAvsLwaRedirectHost(char *avs_lwa_redirect_host, int len)
{
    alexa_context.avs_lwa_redirect_host = avs_lwa_redirect_host;
    alexa_context.avs_lwa_redirect_host_len = len;
}

void AlexaClass::setAvsLwaApiHost(char *avs_lwa_api_host, int len)
{
    alexa_context.avs_lwa_api_host = avs_lwa_api_host;
    alexa_context.avs_lwa_api_host_len = len;
}

void AlexaClass::setAvsClientId(char *avs_client_id, int len)
{
    alexa_context.avs_client_id = avs_client_id;
    alexa_context.avs_client_id_len = len;
}

void AlexaClass::setAvsClientSecret(char *avs_client_secret, int len)
{
    alexa_context.avs_client_secret = avs_client_secret;
    alexa_context.avs_client_secret_len = len;
}

void AlexaClass::setAvsRedirectUri(char *avs_redirect_uri, int len)
{
    alexa_context.avs_redirect_uri = avs_redirect_uri;
    alexa_context.avs_redirect_uri_len = len;
}

void AlexaClass::setAvsProductId(char *avs_product_id, int len)
{
    alexa_context.avs_product_id = avs_product_id;
    alexa_context.avs_product_id_len = len;
}

void AlexaClass::setAvsProductDsn(char *avs_product_dsn, int len)
{
    alexa_context.avs_product_dsn = avs_product_dsn;
    alexa_context.avs_product_dsn_len = len;
}

void AlexaClass::setLoadRefreshTokenHandler( int (*load_refresh_token_handler)(char *refresh_token, uint32_t *refresh_token_len) )
{
    alexa_context.load_refresh_token_handler = load_refresh_token_handler;
}

void AlexaClass::setStoreRefreshTokenHandler( int (*store_refresh_token_handler)(char *refresh_token, uint32_t refresh_token_len) )
{
    alexa_context.store_refresh_token_handler = store_refresh_token_handler;
}

void AlexaClass::setHomePageGeneratorHandler( int (*home_page_generator_handler)(char **content, uint32_t *len) )
{
    alexa_context.home_page_generator_handler = home_page_generator_handler;
}

void AlexaClass::setAuthPageGeneratorHandler( int (*auth_page_generator_handler)(char **content, uint32_t *len) )
{
    alexa_context.auth_page_generator_handler = auth_page_generator_handler;
}

void AlexaClass::setAvsHttp2Host(char *avs_http2_host, int len)
{
    alexa_update_avs_http2_host(avs_http2_host, len);
}

void AlexaClass::setAudioBufferSize(int rx_size, int tx_size)
{
    if (aud_rxbuf != NULL) {
        free(aud_rxbuf);
    }

    if (aud_txbuf != NULL) {
        free(aud_txbuf);
    }

    aud_rxbuf_size = rx_size;
    aud_txbuf_size = tx_size;

    aud_rxbuf = (uint8_t *) malloc (aud_rxbuf_size);
    aud_txbuf = (uint8_t *) malloc (aud_txbuf_size);
}

void AlexaClass::setAudioInitializeHandler(void (*handler)())
{
    codec_init_handler = (void *)handler;
}

AlexaClass Alexa;
