/*

References:

Adafruit LED Backpacks, Page 39 of 74 (wiring)
https://us.rs-online.com/m/d/b79e41ef35af9ade9f1fced30a55a922.pdf 

Arduino Library for HT16K33 4x7segment display
https://github.com/RobTillaart/HT16K33

Demo Display Time (code)
https://github.com/RobTillaart/HT16K33/blob/master/examples/demo_displayTime/demo_displayTime.ino

Arduino Library for Real Time Clock Modules
https://github.com/Makuna/Rtc/wiki

Interfacing DS1302 RTC Module with Arduino UNO R3 (wiring and code)
https://electropeak.com/learn/interfacing-ds1302-real-time-clock-rtc-module-with-arduino/


HT16K33 Backpack to Arduino UNO R3 Terminals
  IO    --> Jumper to 5V
  +     --> 5V
  -     --> GND
  D     --> SDA
  C     --> SCL


DS1302 RTC Module Pins to Arduino UNO R3 Terminals
  VCC       --> 3.3v - 5v
  GND       --> GND
  CLK/SCLK  --> 5
  DAT/IO    --> 4
  RST/CE    --> 2

*/

// Library for DS1302 RTC Module
#include <ThreeWire.h>  
#include <RtcDS1302.h>

// Library for HT16K33 Backpack
#include "HT16K33.h"

// Configure pins for DS1302 RTC Module
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// Instance for HT16K33 Backpack
HT16K33 seg(0x70);

int interval = 500;

void setup () 
{
    Serial.begin(9600);
    Serial.println(__FILE__);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    // Setup for HT16K33 Backpack
    seg.begin();
    Wire.setClock(100000);
    seg.displayOn();
    seg.setDigits(4);

    // Setup for DS1302 RTC Module
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);

    if (dt.Hour() == 0){ // 00:00 --> 12:00AM
      seg.displayTime(12, dt.Minute(), true, false);
      seg.displayExtraLeds(0x06); // upper left point + colon
      delay(interval);
      seg.displayExtraLeds(0x04); // upper left point only
      delay(interval);
    } else if ((dt.Hour() > 0) && (dt.Hour() < 12) ){ // 01:00-11:00 --> 1:00AM-11:00AM
      seg.displayTime(dt.Hour(), dt.Minute(), true, false);
      seg.displayExtraLeds(0x06); // upper left point + colon
      delay(interval);
      seg.displayExtraLeds(0x04); // upper left point only
      delay(interval);
    } else if (dt.Hour() == 12){ // 12:00 --> 12:00PM
      seg.displayTime(12, dt.Minute(), true, false);
      seg.displayExtraLeds(0x0A); // lower left point + colon
      delay(interval);
      seg.displayExtraLeds(0x08); // lower left point only
      delay(interval);
    } else { // 13:00-23:00 --> 1:00PM-11:00PM
      seg.displayTime(dt.Hour() - 12, dt.Minute(), true, false);
      seg.displayExtraLeds(0x0A); // lower left point + colon
      delay(interval);
      seg.displayExtraLeds(0x08); // lower left point only
      delay(interval);
    }
}


