/*******************************************************************************
 * ModbusTCP.h
 *
 * Lightweight Modbus TCP 32-bit slave for Arduino. 
 * 
 * Three MODBUS function codes are supported; reading (0x04) and writing (0x10)
 * of holding registers and function 0x06 for compatiblilty. 
 * 
 * Three Modbus encodings are supported; UInt16, UInt32 and Float (Float and 
 * Double are identical on Arduino Uno). With these three primitive types you
 * can also support Int16, Int32, or packed bits (coils). 
 * 
 * Holding register addressing begins at 40001 and are contiguous.
 * 
 * Data is stored in native MODBUS UInt16 registers. To make it less error 
 * prone, get and set functions are included for each encoding that also do 
 * error checking. If you're crushed for resources feel free to comment out
 * the define statements and write to the registers directly.
 * 
 * Testing
 * This code was developed and tested using modpoll and Inductive Automation 
 * Ignition quick client running in trial mode.
 * 
 * http://www.modbusdriver.com/modpoll.html
 * https://www.inductiveautomation.com/scada-software
 * 
 * Credits
 * 
 * https://code.google.com/p/mudbus/
 * 
 * Thanks to Dee Wykoff and Martin Pettersson for making the Mudbus library 
 * available as a place to start.
 *
 * MIT License
 * Copyright (c) 2014 Ductsoup
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 */

/*******************************************************************************
 * 
 * MODBUS Reference
 * 
 * http://www.csimn.com/CSI_pages/Modbus101.html
 * 
 * Modicon convention notation
 * 
 * 0x = Coil = 00001-09999
 * 1x = Discrete Input = 10001-19999
 * 3x = Input Register = 30001-39999
 * 4x = Holding Register = 40001-49999
 * 
 * Modbus protocol defines a holding register as 16 bits wide; however, there is 
 * a widely used defacto standard for reading and writing data wider than 16 
 * bits. The most common are IEEE 754 floating point, and 32-bit integer. The 
 * convention may also be extended to double precision floating point and 64-bit 
 * integer data.
 * 
 * http://www.freemodbus.org/
 * 
 * ----------------------- MBAP Header --------------------------------------
 * 
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 * <----------- MODBUS PDU (1') ---------------->
 * +-----------+---------------+------------------------------------------+
 * | TID | PID | Length | UID  |Code | Data                               |
 * +-----------+---------------+------------------------------------------+
 * |     |     |        |      |                                           
 * (2)   (3)   (4)      (5)    (6)                                          
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte) 
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 * 
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 * 
 *******************************************************************************/

#ifndef HEADER_ModbusTCP
#define HEADER_ModbusTCP

//
// Note: The Arduino IDE does not respect conditional included
// header files in the main sketch so you have to select your
// hardware there, and here.
//
#ifndef ARDUINO_AMEBA
#define MB_ETHERNET
#endif
//#define MB_CC3000

#ifdef MB_ETHERNET
#include <Ethernet.h>
#endif
#ifdef MB_CC3000
#include <Adafruit_CC3000.h>
#include <SPI.h>
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
#endif

#ifdef ARDUINO_AMEBA

#define MODBUSTCP_PCN001_USE_WIFI

#endif

#ifdef MODBUSTCP_PCN001_USE_WIFI
#include <WiFi.h>
#endif

//
// Application specific, adjust as necessary
//
#define MB_PORT 502
#define MB_REGISTERS_MAX 32
//
// Comment out the features you don't need                            
//
#define MB_Float            
#define MB_UInt32
#define MB_UInt16
//#define MB_DEBUG

class ModbusTCP {
public:
  ModbusTCP(void);
#ifdef MB_ETHERNET  
  void begin(uint8_t mac[6]);
#endif
#ifdef MB_CC3000  
  void begin(const char *ssid, const char *key, uint8_t secmode);
#endif
#ifdef MODBUSTCP_PCN001_USE_WIFI
  int begin(char* ssid);
  int begin(char* ssid, uint8_t key_idx, const char* key);
  int begin(char* ssid, const char *passphrase);
#endif

  void run(void);

#ifdef MB_Float
  int setFloat(uint16_t iAddress, float fValue);
  float getFloat(uint16_t iAddress);
#endif
#ifdef MB_UInt32    
  int setU32(uint16_t iAddress, uint32_t iValue);
  uint32_t getU32(uint16_t iAddress);
#endif
#ifdef MB_UInt16
  int setU16(uint16_t iAddress, uint16_t iValue);
  uint16_t getU16(uint16_t iAddress);
#endif
#ifdef MB_DEBUG
  void printHex(int num, int precision);
  void printMB(char *s, int n);
#endif
private:
  uint16_t mb_reg[MB_REGISTERS_MAX];
  uint8_t mb_adu[260];
#ifdef MB_ETHERNET
    EthernetServer mb_server;
#endif
#ifdef MB_CC3000
    Adafruit_CC3000 mb_client;
    Adafruit_CC3000_Server mb_server;
#endif
#ifdef MODBUSTCP_PCN001_USE_WIFI   
    WiFiServer mb_server;
    WiFiClient clientrequest;
#endif
};
//
// MODBUS Function Codes
//
#define MB_FC_NONE 0
#define MB_FC_READ_COILS 1
#define MB_FC_READ_DISCRETE_INPUT 2
#define MB_FC_READ_REGISTERS 3 // implemented
#define MB_FC_READ_INPUT_REGISTERS 4
#define MB_FC_WRITE_COIL 5
#define MB_FC_WRITE_REGISTER 6 // implemented
#define MB_FC_WRITE_MULTIPLE_COILS 15
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 // implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID          0 // ignored
#define MB_TCP_PID          2 // ignored
#define MB_TCP_LEN          4
#define MB_TCP_UID          6 // ignored
#define MB_TCP_FUNC         7
#define MB_TCP_DATA         8

#endif
