/*
  This sketch shows how to use Amazon Alexa service on Ameba.
  You need these items:
      Ameba x1
      Teensy Audio Shield (SGTL5000) x1
      AOM-6738P-R microphone x1
      button x1

  This sketch requre I2S interface.
  According to Teensy Audio Shield pinout, we connect pins as below:

      Ameba                Audio Shield
      GPIOB_2(SCL)         SCL
      GPIOB_3(SDA)         SDA
      GPIOC_4(I2S_SDRX)    RX
      GPIOC_1(I2S_CLK)     BCLK
      GPIOC_3(I2S_MCK)     MCLK
      GPIOC_2(I2S_SDTX)    TX
      GPIOC_0(I2S_WS)      LRCLK
      3.3V                 3.3V and VOL
      GND                  GND

  Audio shield also needs conntect to microphone.
  Here we use AOM-6738P-R microphone.

  A button to trigger and listen voice command from user.
  This button connect to pin 9

  You need pre-configure AVS account.
  For more information, please refer www.amebaiot.com
*/

#include <WiFi.h>
#include <Alexa.h>
#include <FlashMemory.h>

char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "secretPassword";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

char avs_lwa_redirect_host[] = "amazon.com";
char avs_lwa_api_host[]      = "api.amazon.com";
char avs_client_id[]         = "amzn1.application-oa2-client.ce2f72ffbb5341958632d96ddb317de9";
char avs_client_secret[]     = "243ee6808fe84511e5c25bfa6f1d81b7ddceac6d170eb9297d305e74c5f000e9";
char avs_redirect_uri[]      = "ameba.local";
char avs_product_id[]        = "alexa_on_ameba";
char avs_product_dsn[]       = "123456";
char avs_http2_host[]        = "avs-alexa-na.amazon.com";

#define ALEXA_DATA_PATTERN "ALEXA002"
typedef struct _alexa_data {
    char pattern[8];
    uint32_t refresh_token_len;
    char refresh_token[640];
} alexa_t;
alexa_t *alexa_data;

int audio_rx_bufsize = 1152 * 28 * 3; // 3 seconds buffer
int audio_tx_bufsize = 1152 * 28 * 5; // 5 seconds buffer

int audio_rec_pin = 9;

static char buf[4096];

void setup() {
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");

  WiFi.disablePowerSave();

  // this sketch use huge code space, so we can't use default flash memory address
  FlashMemory.begin(0x1FF000, 0x1000);

  pinMode(audio_rec_pin, INPUT);
  setup_alexa();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      delay(1000);
    }
    Serial.println("Connected to wifi");
  }

  if (digitalRead(audio_rec_pin) == 1) {
    if ( Alexa.audioRecordState() == 0) {
      Alexa.audioRecordStart();
      delay(500);
      while (digitalRead(audio_rec_pin) == 1) {
        delay(100);
      }
      Alexa.audioRecordStop();
    }
  }
  
  delay(100);
}

void setup_alexa() {
  Alexa.setAvsLwaRedirectHost(avs_lwa_redirect_host, sizeof(avs_lwa_redirect_host)-1);
  Alexa.setAvsLwaApiHost(avs_lwa_api_host, sizeof(avs_lwa_api_host)-1);
  Alexa.setAvsClientId(avs_client_id, sizeof(avs_client_id)-1);
  Alexa.setAvsClientSecret(avs_client_secret, sizeof(avs_client_secret)-1);
  Alexa.setAvsRedirectUri(avs_redirect_uri, sizeof(avs_redirect_uri)-1);
  Alexa.setAvsProductId(avs_product_id, sizeof(avs_product_id)-1);
  Alexa.setAvsProductDsn(avs_product_dsn, sizeof(avs_product_dsn)-1);

  Alexa.setLoadRefreshTokenHandler( alexa_load_token_from_flash );
  Alexa.setStoreRefreshTokenHandler( alexa_store_token_to_flash );

  Alexa.setHomePageGeneratorHandler( alexa_homepage_generator );
  Alexa.setAuthPageGeneratorHandler( alexa_authpage_generator );

  Alexa.setAvsHttp2Host(avs_http2_host, sizeof(avs_http2_host)-1);

  Alexa.setAudioBufferSize(audio_rx_bufsize, audio_tx_bufsize);
  Alexa.setAudioInitializeHandler(init_audio_codec);

  Alexa.setAudioTxBuffering(14);

  Alexa.begin();
}

int alexa_load_token_from_flash(char *refresh_token, uint32_t *refresh_token_len)
{
  FlashMemory.read();
  alexa_data = (alexa_t *) FlashMemory.buf;
  // check if data stored in flash is valid
  if (strncmp(alexa_data->pattern, ALEXA_DATA_PATTERN, 8) != 0) {
    // data is not valid
    alexa_data->refresh_token_len = 0;
    return -1;
  } else {
    if (refresh_token != NULL) {
      // data is valid, copy it to refresh_token buffer
      sprintf(refresh_token, "%s", alexa_data->refresh_token);
      *refresh_token_len = alexa_data->refresh_token_len;
    }
    return 0;
  }
}

int alexa_store_token_to_flash(char *refresh_token, uint32_t refresh_token_len)
{
  alexa_data = (alexa_t *) FlashMemory.buf;

  strncpy(alexa_data->pattern, ALEXA_DATA_PATTERN, 8);
  alexa_data->refresh_token_len = refresh_token_len;
  sprintf(alexa_data->refresh_token, "%s", refresh_token);

  FlashMemory.update();
}

int alexa_homepage_generator(char **content, uint32_t *len)
{
  char *p = buf;
  uint32_t link_len;

  p += sprintf(p, "<HTML><BODY>");
  p += sprintf(p, "<a href=\"");

  Alexa.getAuthLink(p, &link_len);
  p += link_len;

  p += sprintf(p, "\"><h1>click here to authorize</h1></a>");
  p += sprintf(p, "</BODY></HTML>");

  *content = buf;
  *len = strlen(buf);

  return 0;
}

int alexa_authpage_generator(char **content, uint32_t *len)
{
  char *p = buf;
  uint32_t link_len;

  p += sprintf(p, "<HTML><BODY>");

  if (!Alexa.isAuthComplete()) {
    p += sprintf(p, "<head><meta http-equiv=\"refresh\" content=\"30\" /></head>");
    p += sprintf(p, "<h1>Authenticating......<br/>This may take about 1 minutes. Please wait</h1>");
  } else {
    p += sprintf(p, "<h1>Authenticating Complete!</h1>");
  }
  p += sprintf(p, "</BODY></HTML>");

  *content = buf;
  *len = strlen(buf);

  return 0;
}

void init_audio_codec()
{
  SGTL5000.begin();
  SGTL5000.setVolume(0.3);
  SGTL5000.inputSelect(AUDIO_INPUT_MIC);
  SGTL5000.micGain(40);
  SGTL5000.unmuteHeadphone();
}
