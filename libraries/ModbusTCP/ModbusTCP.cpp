/*******************************************************************************
 * ModbusTCP.cpp
 */
#include <Arduino.h>
#include "ModbusTCP.h"

ModbusTCP::ModbusTCP(void) : 
#ifdef MB_ETHERNET
mb_server(MB_PORT) 
#endif
#ifdef MB_CC3000
mb_client(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER),
mb_server(MB_PORT)
#endif
#ifdef MODBUSTCP_PCN001_USE_WIFI
mb_server(MB_PORT)
#endif
{
}
#ifdef MB_ETHERNET
void ModbusTCP::begin(uint8_t mac[6]){
  
  Serial.print(F("Requesting DHCP ..."));
  if (Ethernet.begin(mac) == 0) {
    Serial.println("failed");
    while (1);
  }
  else
    Serial.println();

  Serial.print(F("Listening on "));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(thisByte < 3 ? "." : "");
  }
  Serial.print(":");
  Serial.print(MB_PORT);
  Serial.println(" ...");
}
#endif
#ifdef MB_CC3000
void ModbusTCP::begin(const char *ssid, const char *key, uint8_t secmode){

  mb_client.begin();
  Serial.print(F("Connecting to ")); Serial.print(ssid); Serial.println(F(" ..."));
  mb_client.connectToAP(ssid, key, secmode); 
  while (!mb_client.checkDHCP())
      delay(100);
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  mb_client.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv);
  Serial.print(F("Listening on "));
  mb_client.printIPdotsRev(ipAddress);
  Serial.print(F(":")); 
  Serial.println(MB_PORT);
  mb_server.begin();
}
#endif

#ifdef MODBUSTCP_PCN001_USE_WIFI
int ModbusTCP::begin(char* ssid) {
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid);
    if (status == WL_CONNECTED) break;
      printf("Retry connecting to %s\r\n", ssid);
    delay(1000); // retry after 1s
  }
  printf("Connected to %s\r\n", ssid);
  mb_server.begin();
}

int ModbusTCP::begin(char* ssid, uint8_t key_idx, const char* key) {
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, key_idx, key);
    if (status == WL_CONNECTED) break;
    printf("Retry connecting to %s\r\n", ssid);
    delay(1000); // retry after 1s
  }
  printf("Connected to %s\r\n", ssid);
  mb_server.begin();
}

int ModbusTCP::begin(char* ssid, const char *passphrase) {
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, passphrase);
    if (status == WL_CONNECTED) break;
    printf("Retry connecting to %s\r\n", ssid);
    delay(1000); // retry after 1s
  }
  printf("Connected to %s\r\n", ssid);
  mb_server.begin();
}

#endif

void ModbusTCP::run(void){

  int16_t i, iStart, iQty;
  int16_t iTXLen = 0;       // response packet length
  uint8_t *ptr, iFC = MB_FC_NONE, iEC = MB_EC_NONE;
  //  
  // Initialize and check for a request from a MODBUS master
  //
#ifdef MB_ETHERNET
  EthernetClient clientrequest = mb_server.available();
#endif
#ifdef MB_CC3000
  Adafruit_CC3000_ClientRef clientrequest = mb_server.available();
#endif
#ifdef ARDUINO_AMEBA
  if (!clientrequest.connected()) {
    clientrequest = mb_server.available();
  }
#endif

#ifdef MODBUSTCP_PCN001_USE_WIFI
  if(clientrequest.available()) {
    int len = clientrequest.read(mb_adu, sizeof(mb_adu));
    if (len > 0) {
      iFC = mb_adu[MB_TCP_FUNC];
    }
  } else {
    delay(10);
  }

#else
  if(clientrequest.available()) {
    //
    // Retrieve request
    //
    for (i = 0 ; clientrequest.available() ; i++) 
      mb_adu[i] = clientrequest.read();
#ifdef MB_DEBUG   
    printMB("RX: ", word(mb_adu[MB_TCP_LEN],mb_adu[MB_TCP_LEN+1])+6);  
#endif    
    //
    // Unpack the function code
    //
    iFC = mb_adu[MB_TCP_FUNC];
  }
#endif // end of #ifdef ARDUINO_AMEBA
  //
  // Handle request
  //
  switch(iFC) {
  case MB_FC_NONE:
    break;
  case MB_FC_READ_REGISTERS: 
    //
    // 03 (0x03) Read Holding Registers
    //
    // modpoll -m tcp -t 4:float -r 40001 -c 1 -1 192.168.x.x
    //
    //     [TransID] [ProtID-] [Length-] [Un] [FC] [Start--] [Qty----] 
    // RX: 0x00 0x01 0x00 0x00 0x00 0x06 0x01 0x03 0x9C 0x40 0x00 0x02 
    //
    //     [TransID] [ProtID-] [Length-] [Un] [FC] [Bc] [float------------] 
    // TX: 0x00 0x01 0x00 0x00 0x00 0x07 0x01 0x03 0x04 0x20 0x00 0x47 0xF1
    //
    // 123456.0 = 0x00 0x20 0xF1 0x47 (IEEE 754)
    //
    // Unpack the start and length
    //
    ptr = mb_adu + MB_TCP_DATA;
    iStart = word(*ptr++, *ptr++) - 40000;
    iQty = 2*word(*ptr++, *ptr);
    //
    // check for valid register addresses     
    //
    if (iStart < 0 || (iStart + iQty/2 - 1) > MB_REGISTERS_MAX) {
      iEC = MB_EC_ILLEGAL_DATA_ADDRESS;
      break;
    }
    //
    // Write data length
    //
    ptr = mb_adu + MB_TCP_DATA;
    *ptr++ = iQty;
    //
    // Write data
    //
    for (i = 0 ; i < iQty/2 ; i++) {
      *ptr++ = highByte(mb_reg[iStart + i]);
      *ptr++ =  lowByte(mb_reg[iStart + i]);
    }
    iTXLen = iQty + 9;
    break;

  case MB_FC_WRITE_REGISTER:
    //
    // 06 (0x06) Write register 
    //
    ptr = mb_adu + MB_TCP_DATA;   
    iStart = word(*ptr++, *ptr++) - 40000;
    //
    // check for valid register addresses     
    //   
    if (iStart < 0 || (iStart - 1) > MB_REGISTERS_MAX) {
      iEC = MB_EC_ILLEGAL_DATA_ADDRESS;
      break;
    }      
    // Unpack and store data
    //
    mb_reg[iStart] = word(*ptr++, *ptr);
    //
    // Build a response 
    //
    iTXLen = 12;             
    break;

  case MB_FC_WRITE_MULTIPLE_REGISTERS: 
    //
    // 16 (0x10) Write Multiple registers
    //
    // modpoll -m tcp -t 4:float -r 40001 -c 1 -1 192.168.x.x 123456.0
    //
    //     [TransID] [ProtID-] [Length-] [Un] [FC] [Start--] [Qty----] [Bc] [float------------] 
    // RX: 0x00 0x01 0x00 0x00 0x00 0x0B 0x01 0x10 0x9C 0x40 0x00 0x02 0x04 0x20 0x00 0x47 0xF1
    //
    // 123456.0 = 0x00 0x20 0xF1 0x47 (IEEE 754)
    //
    // Unpack the start and length
    //
    ptr = mb_adu + MB_TCP_DATA;   
    iStart = word(*ptr++, *ptr++) - 40000;
    iQty = 2*word(*ptr++, *ptr);      
    //
    // check for valid register addresses     
    //   
    if (iStart < 0 || (iStart + iQty/2 - 1) > MB_REGISTERS_MAX) {
      iEC = MB_EC_ILLEGAL_DATA_ADDRESS;
      break;
    }
    //
    // Unpack and store data
    //
    ptr = mb_adu + MB_TCP_DATA+5;
    // todo: check for valid length
    for (i = 0 ; i < iQty/2 ; i++) {
      mb_reg[iStart + i] = word(*ptr++, *ptr++);
    }
    //
    // Build a response 
    //
    iTXLen = 12;
    break;

  default:
    iEC = MB_EC_ILLEGAL_FUNCTION;
    break;    
  }
  //
  // Build exception response if necessary because we were too
  // lazy to do it earlier. Other responses should already be
  // built.
  //
  if (iEC) {
    ptr = mb_adu + MB_TCP_FUNC;
    *ptr = *ptr++ | 0x80;        // flag the function code
    *ptr = iEC;                  // write the exception code
    iTXLen = 9;
  }
  //
  // If there's a response, transmit it
  //
  if (iFC) {
    ptr = mb_adu + MB_TCP_LEN;    // write the header length
    *ptr++ = 0x00;
    *ptr = iTXLen - MB_TCP_UID;  
    clientrequest.write(mb_adu, iTXLen); // send it        
#ifdef MB_DEBUG   
    printMB("TX: ", word(mb_adu[MB_TCP_LEN], mb_adu[MB_TCP_LEN+1]) + MB_TCP_UID);  
#endif                                                
  }
}

#ifdef MB_Float
int ModbusTCP::setFloat(uint16_t iAddress, float fValue) {

  int iRet; 
  union {
    float f;
    uint16_t w[2];
  } 
  fw;

  for (iRet = 0 ; !iRet ; iRet = 2) {
    if ((iAddress -= 40001) < 0 || (iAddress + 1) >= MB_REGISTERS_MAX) // invalid address
      break;
    fw.f = fValue;
    mb_reg[iAddress++] = fw.w[0];
    mb_reg[iAddress] = fw.w[1]; 
  }
  return(iRet);  
}
float ModbusTCP::getFloat(uint16_t iAddress) {

  union {
    float fRet;
    uint16_t w[2];
  } 
  fw;

  if ((iAddress -= 40001) < 0 || (iAddress + 1) >= MB_REGISTERS_MAX) // invalid address
    fw.fRet = NULL;
  else {
    fw.w[0] = mb_reg[iAddress++];  
    fw.w[1] = mb_reg[iAddress];    
  }
  return(fw.fRet);  
}
#endif
#ifdef MB_UInt32
int ModbusTCP::setU32(uint16_t iAddress, uint32_t iValue) {

  int iRet; 
  for (iRet = 0 ; !iRet ; iRet = 2) {
    if ((iAddress -= 40001) < 0 || (iAddress + 1) >= MB_REGISTERS_MAX) // invalid address
      break;
    mb_reg[iAddress++] = iValue & 0xffff;
    mb_reg[iAddress] = iValue >> 16;
  }
  return(iRet);  
}
uint32_t ModbusTCP::getU32(uint16_t iAddress) {

  uint32_t lRet; 

  if ((iAddress -= 40001) < 0 || (iAddress + 1) >= MB_REGISTERS_MAX) // invalid address
    lRet = NULL;
  else {
    lRet = (long(mb_reg[iAddress++]) | (long(mb_reg[iAddress+1]) << 16));
  }
  return(lRet);  
}
#endif
#ifdef MB_UInt16
int ModbusTCP::setU16(uint16_t iAddress, uint16_t iValue) {

  int iRet; 
  for (iRet = 0 ; !iRet ; iRet = 1) {
    if ((iAddress -= 40001) < 0 || (iAddress >= MB_REGISTERS_MAX)) // invalid address
      break;
    mb_reg[iAddress] = iValue;
  }
  return(iRet);  
}
uint16_t ModbusTCP::getU16(uint16_t iAddress) {

  uint16_t iRet; 

  if ((iAddress -= 40001) < 0 || (iAddress >= MB_REGISTERS_MAX)) // invalid address
    iRet = NULL;
  else
    iRet = mb_reg[iAddress];

  return(iRet);  
}
#endif
#ifdef MB_DEBUG
void ModbusTCP::printHex(int num, int precision) {

  char tmp[16];
  char format[128];

  sprintf(format, "0x%%.%dX ", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}
void ModbusTCP::printMB(char *s, int n) {

  int i;

  Serial.print(s);
  for (i = 0 ; i < n ; i++) {
    printHex(mb_adu[i], 2); 
    if (i == 6) Serial.print("- ");
    if (i == 7) Serial.print("- ");      
  }
  Serial.println("");      
}
#endif



