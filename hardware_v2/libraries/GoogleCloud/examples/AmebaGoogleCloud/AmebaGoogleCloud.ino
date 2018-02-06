#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define count   100

extern "C"{   
  extern char *jwt_generator(const unsigned char *private_key, char *project_id, int expiration_seconds);
}


WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);


char ssid[] = "ssid";     // your network SSID (name)
char pass[] = "pass";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

char GOOGLE_MQTT_SERVER[] = "mqtt.googleapis.com";
int  GOOGLE_MQTT_PORT = 8883;

char project_id[] = "amebago-193913";
char registry_id[] = "amebago-registry";
char device_id[] = "amebago-rs256-device";

char clientUser[] = "unused";
char *clientPass;
char *mqtt_id;
char *pub_topic;
char payload[64];



/* root CA can be download here:
 *  https://pki.google.com/GIAG2.crt
 */
char* rootCABuff  = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
"-----END CERTIFICATE-----\n";


/* Fill your private.pem.key with LINE ENDING */
char *privateKeyBuff = \
"-----BEGIN PRIVATE KEY-----\n" \

"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC4Zmd6c4ahWRJ1\n" \
"OkN/BhyusnJgfOdlr9P9417q3mdEkv9d8i0uBuoDUQCBj6ue1PfJ0OEtxwyZGDkH\n" \
"UmP88tO88s9lSZ19D+Czv002XjFn6hUyzZa3hbSpSMD90iUOxLVeMsk4RQ7cOfRG\n" \
"ToKL2t54KUc2GFlGQ8jlvP8gLFmlDjftAGphZNmNkwXPmvwov812CdaA3mZ+YBMj\n" \
"PLFDRYjE5bt2uf2+Cm57VRmPb8ONuAaTPgsLRRNGj+GJOTweMKV54TEPga0u7101\n" \
"LqrofShdNMnMS4ebGzlK0RPaPxczdjHrLC9jHrG0bJDjSJRq4YaVx/zXbzbRribt\n" \
"63NUkCT1AgMBAAECggEAXa/hqSwi3b0UjKzSeCoRzoxpYi5zno1rxpWLtwbSLtwE\n" \
"lKWjYLwwjwjLmgf1qRgI4OeYUJrOAsZ0ywyIMo7pFxnCV3LEajLz9j8eqp3GukYL\n" \
"CSm9BncPJ+cH7q2jGFLG1xo0c7taZnenbUUcPJQx7ZkDTi+mw/VSj66rbJw724h1\n" \
"S+oeDbwEKv3ax1JA19iGOeL736d69EegV0+4cFC50gcxyVkwyfRapx0BYOPbUOo4\n" \
"z0vpSeUKzBqKHmRq2O7g5F5TU2kopOVvVp9HbbG4UGah1BHDzwaiP409B/xmvAOm\n" \
"q2rm3GuN5KX81eFkBlDjomK0GjopeX01Xla4a0VL7QKBgQDrwc2dgUO1Jdv6RU3L\n" \
"7onbPxev6tD8I1H4toexOVg7/2x4C4WS/g7lCED2ORyKZgqpIw0IIS8OguMIm9Mg\n" \
"AaUJQpqSpshFq2slkMPEvL8vt+BKsYKz36xFywijcG4zV3GZELTdVu0v/25++QMb\n" \
"mOUgA5Pq9WKT+5qq5B5JQ8IdzwKBgQDIO7ezrovrT6hsbYgGu4DebeSk4s2DPLRg\n" \
"AzzDdD2Ypl/DLhr5nzqdixmL//uW6qKQn52TErGIqkMzGtkz+TofVt+Yynh3xjRy\n" \
"16P4NUBfsy9YMDcSWwpwWW35IFudGZ86bJZJgtRf+f0otlG6W7dcAVIqhlkXdM4e\n" \
"/ULQU7Il+wKBgCx3ZFnFzMh4+JGuyqqhNj01HDmg94PnAYoAm31QzJSca5AE1E/S\n" \
"PWrzcJVAVmLANliKdOXIpIB/LWUtRtftl3w0pMTuUi3Z1B7EvDf6RbExZEuSSY21\n" \
"rV+ImPuCtDZY0uNE5GgvAhOggO3P98cXwneUVSzm1Y4F0blTx2aYMh+/AoGAEa5M\n" \
"Q16HVmj7S0/Esit+bqWvievJD+ydVNkUVYH/KmqOjDKXCTHJQD4XLGiXM7VWU4T0\n" \
"qhb9fD7kni+hvFgmjLvkFJ7UUmc7HGT0QqeZHpo49QWU51cIrfEHp/b2gAHSMJuE\n" \
"DcuyqyLs+tpWjykoIMSxF7YzScHzrYLZkoHBel8CgYEAja6UGqcjC78mB4KErbCg\n" \
"zA7EvYUmAiRdBkAxMvNDj54YIdIijP0hadJzcDwy2S8A3ckWJglMLxSJ1ghc5v/K\n" \
"SygvyhLc5h5PTSNnElKr//7OLXPYalF/ACcwCum/Ppi6UJcHKBSW+ZGnt+Akennb\n" \
"+BYcrelb7Sulr2Q5EBU6f1k=\n" \
"-----END PRIVATE KEY-----\n";

void setup() {
  
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");
  
  Serial.println("Init MQTT");
  wifiClient.setRootCA((unsigned char*)rootCABuff);      
  wifiClient.setClientCertificate(NULL, (unsigned char*)privateKeyBuff);

}

void loop() {
  boolean ret = 0;  
  
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
    {
      delay(1000);
    }
    Serial.println("Connected to wifi");
  }

  if(ret == 0){      
    Serial.println("Ready for Publishing");
  
    mqtt_id = (char *)malloc(strlen("projects/") + strlen(project_id) + strlen("/locations/us-central1/registries/")
      + strlen(registry_id) + strlen("/devices/") + strlen(device_id) + 1);
    sprintf(mqtt_id, "projects/%s/locations/us-central1/registries/%s/devices/%s", project_id, registry_id, device_id);
      
    clientPass = jwt_generator((unsigned char*)privateKeyBuff, project_id, 3600*1);
  
    pub_topic = (char *)malloc(strlen("/devices/") + strlen(device_id) + strlen("/events") + 1);
    sprintf(pub_topic, "/devices/%s/events", device_id);    
    
    client.setServer(GOOGLE_MQTT_SERVER, GOOGLE_MQTT_PORT);
    client.setPublishQos(MQTTQOS1);
    client.waitForAck(true);
  }
  
  if (client.connect(mqtt_id, clientUser, clientPass) )
  { 
      for(int i = 0; i < count; i++){
        Serial.println("client connecting...");
        memset(payload, 0x0, 64);
        sprintf(payload, "This is Ameba's %d message!!", i);                     
        printf("Publishing the payload \"%s\" with len: %d\r\n", payload, strlen(payload));      
        ret = client.publish(pub_topic, payload);    
        if(ret == 1)
          printf("client publish successfully!!! ret = %d\r\n",ret);            
        else{
          printf("client publish unsuccessfully!!! ret = %d\r\n",ret);
          break;
        }                     
        if (!client.connected()) {
          Serial.println("MQTT disconnect with server");
          break;
        }

        delay(1000);
      }
      client.disconnect();  
  }
  
  free(mqtt_id);
  free(pub_topic);
  
}


