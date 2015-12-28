#ifndef wificlient_h
#define wificlient_h
#include "Print.h"
#include "Client.h"
#include "IPAddress.h"
#include "server_drv.h"
class WiFiClient : public Client {

public:
	
  WiFiClient();
  WiFiClient(uint8_t sock);

  uint8_t status();
  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();

  friend class WiFiServer;

  using Print::write;

private:
	int _sock;
	ServerDrv clientdrv;
	bool _is_connected;
	uint8_t data[DATA_LENTH];
};

#endif
