/*
 * Example showing RTC sync from NTP time source
 *
 */

#include <Wire.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <DS1307RTC.h>

int status = WL_IDLE_STATUS;
char ssid[] = "yourNetwork";          // your network SSID (name)
char pass[] = "secretPassword";       // your network password

// NTP Servers:
char timeServer[] = "time.stdtime.gov.tw";
//char timeServer[] = "clock.stdtime.gov.tw";
//char timeServer[] = "tick.stdtime.gov.tw";
//char timeServer[] = "tock.stdtime.gov.tw";
//char timeServer[] = "watch.stdtime.gov.tw";
//IPAddress timeServer(118, 163, 81, 61); // time.stdtime.gov.tw
//IPAddress timeServer(211, 22, 103, 158); // clock.stdtime.gov.tw
//IPAddress timeServer(118, 163, 81, 62); // tick.stdtime.gov.tw
//IPAddress timeServer(211, 22, 103, 157); // tick.stdtime.gov.tw
//IPAddress timeServer(118, 163, 81, 63 ); // tick.stdtime.gov.tw

const int timeZone = 8;     // Beijing Time, Taipei Time
//const int timeZone = 1;   // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
unsigned int localPort = 2390;  // local port to listen for UDP packets

void setup() 
{
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    if (status == WL_CONNECTED) break;
    // wait 1 seconds for retry
    delay(1000);
  }

  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);

  // calibrate RTC time from NTP time
  if (timeStatus() != timeNotSet) {
    tmElements_t tm;
    tm.Hour = hour();
    tm.Minute = minute();
    tm.Second = second();
    tm.Day = day();
    tm.Month = month();
    tm.Year = CalendarYrToTm(year());
    // configure RTC time
    RTC.write(tm);
  }
}


void loop()
{
  tmElements_t tm;

  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  Serial.println("Transmit NTP Request");
  Udp.setRecvTimeout(1500);
  sendNTPpacket();
  if ( Udp.read(packetBuffer, NTP_PACKET_SIZE) > 0 ) {
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
  } else {
    Serial.println("No NTP Response :-(");
    return 0; // return 0 if unable to get the time
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket()
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(timeServer, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
