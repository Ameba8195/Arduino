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

public:
    // below API are needed to setup before run Alexa service
	void setAvsRefreshToken(char *avs_refresh_token, int len);
    void setAvsClientId(char *avs_client_id, int len);
    void setAvsClientSecret(char *avs_client_secret, int len);
    void setAvsHttp2Host(char *avs_http2_host, int len);	

private:


};

extern AlexaClass Alexa;

#endif
