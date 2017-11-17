#include <WiFi.h>
#include <GoogleCloud.h>

char ssid[] = "free_or_die";     // your network SSID (name)
char pass[] = "0928513843";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)


unsigned char * project_id = ""
unsigned char * registry_id = ""
unsigned char * device_id = ""

/* Fill your private.pem.key with LINE ENDING */
unsigned char *private_key = \
"-----BEGIN PRIVATE KEY-----\r\n" \
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC8AQz4dsBWnZeN\r\n" \
"S6tATz6aZ//wAA7/Tbo6jmpIuUxvj33aSlD7MTzsvKf0yQoPDhTkFgQ6oHaCx2N/\r\n" \
"Tl/lBk6ZFeK6im48YaXXqnqV542qQ45U9SnznmBCnvZi9jLdYGIxmlO52BzNMq9i\r\n" \
"i2dPOvpczFTR1YAYVRk0CZ/L65vK4V5u/LrRMETfiNCYy8B+R1cWG4iSzPrsHOMY\r\n" \
"yL35bU+YjiUGauzFHMD6PdoPIzD9DZqPINg6sZax0OyXX+DyA0yXU+9IxmYBOQ2r\r\n" \
"F1wO2QgsWN3zR9z+WNl4+CKiHt13WeRamA0BeZgr3+K3mt5a5SGeUt8g+/rQShLs\r\n" \
"zFodooWLAgMBAAECggEBAI6X0WmmUA2h66+cY+Ab+qSMGt/Ce52CK6x8FfrgqCu5\r\n" \
"TsBBO5i56La6BD2yObHfgVLZrA13ZUudxUZqgGsp1HVh1xybIC8l9O5VfDK0dIte\r\n" \
"P9cKN+5yAhqljMTUbRHhJ7jtj3FWi8nLFqEMkKfUWM4HoJGieEx8RuhkIFI/emNu\r\n" \
"LTJaRVfFhOtCjtWHY+sWR+ccjbr7oH6HImQAZhCtRLPomU1BCnNpQQstxPthF4Mp\r\n" \
"Fp+XG7FXo2JjsZx3cWr3atlCl6qo4Ls1AbV/z1odoSRP2NYYAO9KSg691DRqvABR\r\n" \
"ZRGO9rN2y2AJpgDgh0sfywY5TfHOwsnQKTRTz9O1oTECgYEA23RnKPz+r9DfhmXs\r\n" \
"norSIV4yt5ahb3chctMXsCIqYdyx4j7xVqeDgEsHQKgWWI2DwTMUlpYDp9pmnVZH\r\n" \
"jG77TGqm/Ze4SDqMfOXsC9c2rhz3LdLZl8X0z8cO5bUfSu+gKgMszqApK7tsNogg\r\n" \
"GQfegm3AcO2jHcW8O7LKc1YKngkCgYEA20/fvSwuGOCUhhmaikvdZJPu3l4apQGW\r\n" \
"0zd/scP7i5ILZj0rlVH2SH65rPZP9pFvEbmLrkNumPOnPxuOGuO3aA4okcSqCojY\r\n" \
"9zRlHAP2aBRE6HRaj31QgbBYlHuTg6lNeWnUpVsUc1i1muDOzi82CJvSowvGT98w\r\n" \
"l2siiZGrK/MCgYBFXmotfqtTEAdYCGffRapeedQ2TBsDjPi5vxhByMr03Doudg77\r\n" \
"gUAEEbNdMMS/gRwgufDMGOdW66Z8HYXIUlyFL0FR79+ElV1LLbKLm5/hMa7AgWHe\r\n" \
"o+hatWV0HuihW1WZch97wQzCkiAGrMjYoLApMmAeUj+5bYOuqzlrc8svYQKBgCPu\r\n" \
"+GfUf1EQKwW12D5ko8fA/Vht7suucPn2k/OaevKpYTjFOQjkPwgty1ptmTh7J8mt\r\n" \
"rxdEcTCaa4LJjwUU3o4sE7WelYXYWkRWz2JBu/PdrlSwC86K58/4But9O/RMnycP\r\n" \
"kgnt3HHxzP+tafUagy3x6hdQ7JNVrG6C3UvRFEzFAoGBANA3Lp1bHw2uEyCZp9Ua\r\n" \
"aK4kdocE32so+wuMNZI/bX/CdPV/UEGSj2kOPvohms4U6yHnTcyjnru72ZUUunZy\r\n" \
"R+17rZQENFRPqEgbGwvXUSAJ3mCa1hLuRBqiMMGqbv25ja3UaDo0JmBRz75LJdn0\r\n" \
"sL/W3B8EkjtGnXC9OkD/rGtn\r\n" \
"-----END PRIVATE KEY-----\r\n";

int ret = -1;

void setup() {
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");

  gc_class.setPrivatekey(private_key);
  
  while(ret != SUCCESS)
  {
    ret = gc_class.connect(project_id, registry_id, device_id);
     delay(1000);
  }
   
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
    {
      delay(1000);
    }
    Serial.println("Connected to wifi");
  }

  if(gc_class.isconnected())
    gc_class.publish();
  else
    printf("\r\nMQTT disconnect with server\r\n");

  delay(1000);
}


