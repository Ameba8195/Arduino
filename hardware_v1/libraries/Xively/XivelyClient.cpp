#include <Xively.h>
#include <HttpClient.h>
#include <CountingStream.h>

XivelyClient::XivelyClient(Client& aClient)
  : _client(aClient)
{
}

int XivelyClient::put(XivelyFeed& aFeed, const char* aApiKey)
{
  	HttpClient http(_client);
  	char path[30];
  	buildPath(path, aFeed.id(), "json");
  	http.beginRequest();
  	int ret = http.put("api.xively.com", path);
  	if (ret == 0){
    	http.sendHeader("X-ApiKey", aApiKey);
    	http.sendHeader("User-Agent", "Xively-Arduino-Lib/1.0");    

    	CountingStream countingStream; // Used to work out how long that data will be
    	for (int i =kCalculateDataLength; i <= kSendData; i++)
    	{
      		Print* s;
      		int len =0;
      		if (i == kCalculateDataLength){
        		s = &countingStream;
      		}
      		else{
        		s = &http;
      		}
      		len = s->print(aFeed);
      		if (i == kCalculateDataLength){
        		http.sendHeader("Content-Length", len);
      		}
    	}
    	http.endRequest();

    	ret = http.responseStatusCode();
    	if ((ret < 200) || (ret > 299))
    	{
      		if (ret > 0){
        		ret = ret * -1;
      		}
    	}
    	http.flush();
    	http.stop();
  	}
  	return ret;
}

void XivelyClient::buildPath(char* aDest, unsigned long aFeedId, const char* aFormat)
{
  	char idstr[12]; 
  	strcpy(aDest, "/v2/feeds/");
  	char* p = &idstr[10];
  	idstr[11] = 0;
  	for(*p--=aFeedId%10+0x30;aFeedId/=10;*p--=aFeedId%10+0x30);
  	strcat(aDest, p+1);
  	strcat(aDest, ".");
  	strcat(aDest, aFormat);
}

int XivelyClient::get(XivelyFeed& aFeed, const char* aApiKey)
{
  	HttpClient http(_client);
  	char path[30];
  	buildPath(path, aFeed.id(), "csv");
  	http.beginRequest();
  	int ret = http.get("api.xively.com", path);
  	if (ret == 0){
    	http.sendHeader("X-ApiKey", aApiKey);
    	http.sendHeader("User-Agent", "Xively-Arduino-Lib/1.0");    
    	http.endRequest();

    	ret = http.responseStatusCode();
    	if ((ret < 200) || (ret > 299)){
      		if (ret > 0){
        		ret = ret * -1;
      		}
    	}
    	else{
      		http.skipResponseHeaders();
      		int idIdx = 0;
      		unsigned long idBitfield = 0;
      		for (int i =0; i < aFeed.size(); i++){
        		idBitfield |= 1 << i;
      		}

      		while ((http.available() || http.connected())){
        		if (http.available()){
          			char next = http.read();
          			switch (next)
          			{
          				case ',':
            				http.find(",");
            				for (int i =0; i < aFeed.size(); i++)
            				{
              					if ((idBitfield & 1<<i) && (aFeed[i].idLength() == idIdx))
              					{
                					aFeed[i].updateValue(http);
                					next = '\n';
              					}
            				}
            				while ((next != '\r')  && (next != '\n') && (http.available() || http.connected()))
							{
              					next = http.read();
            				}
          				case '\r':
          				case '\n':
            				idIdx = 0;
            				for (int i =0; i < aFeed.size(); i++)
            				{
              					idBitfield |= 1 << i;
            				}
            				break;
          				default:
            				for (int i =0; i < aFeed.size(); i++)
            				{
              					if (!(idBitfield & 1<<i) || (aFeed[i].idChar(idIdx) != next))
              					{
                					idBitfield &= ~(1<<i);
              					}
            				}
            				idIdx++; // onto the next character in the ID
            				break;
          			};
        		}
      		}
      		delay(10);
    	}
    	http.stop();
  	}
  	return ret;
}
