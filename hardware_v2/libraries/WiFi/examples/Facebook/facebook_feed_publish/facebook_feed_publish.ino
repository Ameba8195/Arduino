/*
  In this sketch, it assumed you already have a user access token. Using this token you can test if it works to post something on facebook.
  
  If you don't have access token, please reference below to get one:
    https://developers.facebook.com/tools/explorer/
  And you need these access right to publish on feed:
    https://developers.facebook.com/docs/graph-api/reference/v2.8/user/feed

  If you already have one, using link blow to check the access right:
    https://developers.facebook.com/tools/debug/accesstoken
  You can also extend the expire time of access token in this page.
*/

#include <WiFi.h>

char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "secretPassword";  // your network password
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

char server[] = "graph.facebook.com";    // facebook graph api service

char access_token[] = "EAAEcZAcqQqtUBAAN0YbAemjZCwvmknqjocENiPwUX92TR7Q54jiKrtyEOR8p2EgfR1RQz3uWeDmEQt9oNYOMaRBqGci55CD9chdXcCQXu1jwXPvLpvPZAMeZBVxWZA94sCEZBVN93ZADDe6XkaJpDxJlvdTF0zATFMZD";

WiFiSSLClient client;

#define FEED_ID "me"
//#define FEED_ID "1210426395661463"

#define MESSAGE_LIST_SIZE 5
int message_index = 0;
char *message_list[MESSAGE_LIST_SIZE] = {
  "HelloWorld",
  "This%20is%20an%20ameba%20application%20for%20facebook",
  "%E9%80%99%E6%98%AF%E9%98%BF%E7%B1%B3%E5%B7%B4%E7%9A%84%E8%87%89%E6%9B%B8%E6%87%89%E7%94%A8",
  "%E9%98%BF%E7%B1%B3%E5%B7%B4%E7%9A%84%E8%84%B8%E4%B9%A6%E5%BA%94%E7%94%A8%E6%9C%89%E7%82%B9%E6%84%8F%E6%80%9D",
  "For%20detail%20please%20reference%20http%3A%2F%2Fwww.amebaiot.com%2F",
};

void setup() {
  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to wifi");
}

void loop() {
  while (1) {
    if (!client.connect(server, 443)) {
      Serial.println("Connect to server failed. Retry after 1s.");
      client.stop();
      delay(1000);
      continue;
    }

    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("POST /v2.11/" FEED_ID "/feed?access_token=");
    client.print(access_token);
    client.print("&message=");
    client.print(message_list[message_index]);
    message_index = (message_index + 1) % MESSAGE_LIST_SIZE;
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println();

    while (!client.available()) delay(100);
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    client.stop();
    break;
  }

  delay(60 * 1000); // delay 1 minute for another publish
}