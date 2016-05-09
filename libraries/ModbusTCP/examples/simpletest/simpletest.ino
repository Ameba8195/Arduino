/*

 Simple Test

 You can find modpoll command line tool here:
   http://www.modbusdriver.com/modpoll.html

 */

#include "ModbusTCP.h"

char ssid[] = "yourNetwork";     // your network SSID (name)
char pass[] = "secretPassword";  // your network password

// Register map for this example
#define S1_FloatConstant 40001 // modpoll -m tcp -t 4:float -r 40001 [ip_addr] 
#define S1_CurrentMillis 40003 // modpoll -m tcp -t 4:int -r 40003 [ip_addr]
#define S1_FeetPerMile   40005 // modpoll -m tcp -t 4 -r 40005 [ip_addr]

ModbusTCP m;

void setup(void)
{
  // set some initial values
  m.setFloat(S1_FloatConstant, PI);
  m.setU32(S1_CurrentMillis, millis());
  m.setU16(S1_FeetPerMile, 5280);
  
  m.begin(ssid, pass);
}

void loop(void)
{
  // Process MODBUS requests on every scan
  m.run();
  
  // Update the MODBUS registers  
  m.setU32(S1_CurrentMillis, millis());
}



