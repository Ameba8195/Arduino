#ifndef _ALEXA_H_
#define _ALEXA_H_

#if defined(BOARD_RTL8710)
#error "RTL8710 do not support I2S"
#endif

#include <inttypes.h>
#include <AmebaSGTL5000.h>

class AlexaClass {
public:
    AlexaClass();
    ~AlexaClass();

    void begin();

    void setAudioTxBuffering(uint32_t threshold);

    int getAuthLink(char *link, uint32_t *link_len);
    int isAuthComplete();

    int  audioRecordState();
    void audioRecordStart();
    void audioRecordStop();

public:
    // below API are needed to setup before run Alexa service

    void setAvsLwaRedirectHost(char *avs_lwa_redirect_host, int len);
    void setAvsLwaApiHost(char *avs_lwa_api_host, int len);
    void setAvsClientId(char *avs_client_id, int len);
    void setAvsClientSecret(char *avs_client_secret, int len);
    void setAvsRedirectUri(char *avs_redirect_uri, int len);
    void setAvsProductId(char *avs_product_id, int len);
    void setAvsProductDsn(char *avs_product_dsn, int len);

    void setLoadRefreshTokenHandler( int (*load_refresh_token_handler)(char *refresh_token, uint32_t *refresh_token_len) );
    void setStoreRefreshTokenHandler( int (*store_refresh_token_handler)(char *refresh_token, uint32_t refresh_token_len) );

    void setHomePageGeneratorHandler( int (*home_page_generator_handler)(char **content, uint32_t *len) );
    void setAuthPageGeneratorHandler( int (*auth_page_generator_handler)(char **content, uint32_t *len) );

    void setAvsHttp2Host(char *avs_http2_host, int len);

    void setAudioBufferSize(int rx_size, int tx_size);
    void setAudioInitializeHandler(void (*codec_init_handler)());

private:
    void *context;
    uint8_t *aud_rxbuf;
    uint32_t aud_rxbuf_size;
    uint8_t *aud_txbuf;
    uint32_t aud_txbuf_size;
    void *codec_init_handler;
};

extern AlexaClass Alexa;

#endif
