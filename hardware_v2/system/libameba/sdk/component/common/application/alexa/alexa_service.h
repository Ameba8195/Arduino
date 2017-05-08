#ifndef _ALEXA_SERVICE_H_
#define _ALEXA_SERVICE_H_

#include <inttypes.h>

#include "alexa/alexa_service.h"
#include "alexa/alexa_audio_service.h"
#include "alexa/alexa_mdns_service.h"
#include "alexa/alexa_http_service.h"
#include "alexa/alexa_http2_service.h"

#define ALEXA_MDNS_DEVICE_NAME  "alexa"
#define ALEXA_MDNS_SERVICE_NAME "alexa_example"

#define AVS_PRODUCT_ID        "alexa_on_ameba"
#define AVS_PRODUCT_DSN       "123456"

#define AVS_CLIENT_ID         "amzn1.application-oa2-client.ce2f72ffbb5341958632d96ddb317de9"
#define AVS_CLIENT_SECRET     "243ee6808fe84511e5c25bfa6f1d81b7ddceac6d170eb9297d305e74c5f000e9"

#define AVS_REDIRECT_URI      "ameba.local"

#define AVS_LWA_REDIRECT_HOST "amazon.com"
#define AVS_LWA_API_HOST      "api.amazon.com"
#define AVS_HTTP2_HOST        "avs-alexa-na.amazon.com"

// AVS mp3 audio files has 1152 bytes for each frame after decoding
#define ALEXA_I2S_DMA_PAGE_SIZE	1152

void alexa_update_avs_http2_host(char *host, int len);
void alexa_update_avs_token(char *token, int len);

#endif
