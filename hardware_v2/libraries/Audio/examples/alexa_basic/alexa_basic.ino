/*
  This sketch shows how to use Amazon Alexa service on Ameba.
  You need these items:
      Ameba RTL8195AM x1
      Realtek Audio Shield (ALC5680) x1      
      Speaker x1

  This sketch requre I2S interface.
  According to Teensy Audio Shield pinout, we connect pins as below:

      Ameba                Audio Shield
      GPIOC_5(IRQ_VOICE)   BUTTON
      GPIOE_5(LED)         LED      
      GPIOC_0(I2S_WS)      LRCLK
      GPIOC_1(I2S_CLK)     BCLK
      GPIOC_2(I2S_SDTX)    TX      
      3.3V                 3.3V and VOL
      GND                  GND

  Audio shield also needs conntect to Powered Spekaer.
  
  A button to trigger and listen voice command from user.
  This button is on  Realtek Audio Shield.

  You need pre-configure AVS account.
  For more information, please refer www.amebaiot.com
*/

#include <WiFi.h>
#include <Alexa.h>
#include <FlashMemory.h>

char ssid[] = "ssid";     // your network SSID (name)
char pass[] = "password";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

char avs_refresh_token[]     = "Atzr|IwEBIIPONbIeIbsgOXzSs557Ug8yT2yVkoXBpeMepDLQJ_9NA9SOTUuiQaHU0ZIB_rCxvIjpYlSI9SabI8WIk946zsYx6qMzFyWj7Gf6ATmCgYxldZ7imL100NfqPk0UreuUXD5m9lYc0aI2U2tMZ2YLgLHgnOZuIbtQwGWoVn_HV0lhVXfubnZ7ZFbII5l9ZLLNB9dRswQ0r_geRDyCBucP8c_ZZMcDwVadtvkAku9eqF8CI0CqviJydcvy1T0Kq3OZj_aMEinoBMWM9eU6yrTn_-Er219AAXkP1t4f4NIeZOlBgkFvvxDplvHzdbIQ_dGNvEeg3pAUGNX5E10umwJgdbr9XYVO1KWvshSweZiU6BGLn9bvsaMmdV90cB453eYSeom953Gy_5ly7U0xg0XOeQPMPTziMBwK6JxRi420LdFc2TzAWFPwwFvucAdqxPdhHDz_lJdpK-Fa2GqEVjAM1pOd1OIpTgry2nIFcOR6qwaNt_-kpCjNPYiBhqhPQzFtCjcew_em9q23fyduBFhu1H2FPJHppG7Mh9bHaCFCbBGYgQ";
char avs_client_id[]         = "amzn1.application-oa2-client.b4c18819f8ea421fafc1208818b25670";
char avs_client_secret[]     = "b46599cb915d66268ec94645a07471b9dc53cb33e3a04f12e4c95a33730c928c";
char avs_http2_host[]        = "avs-alexa-na.amazon.com";

void setup() {
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");

  WiFi.disablePowerSave();

  // this sketch use huge code space, so we can't use default flash memory address
  //FlashMemory.begin(0x1FF000, 0x1000);

  
  setup_alexa();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
    {
      delay(1000);
    }
    Serial.println("Connected to wifi");
  }

  delay(100);
}

void setup_alexa() {
  Alexa.setAvsRefreshToken(avs_refresh_token, sizeof(avs_refresh_token)-1);
  Alexa.setAvsClientId(avs_client_id, sizeof(avs_client_id)-1);
  Alexa.setAvsClientSecret(avs_client_secret, sizeof(avs_client_secret)-1);
  Alexa.setAvsHttp2Host(avs_http2_host, sizeof(avs_http2_host)-1);

  Alexa.begin();
}

