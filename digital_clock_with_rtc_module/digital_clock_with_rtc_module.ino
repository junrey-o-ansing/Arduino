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

How to use a Buzzer
https://www.ardumotive.com/how-to-use-a-buzzer-en.html


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

Buzzer ti Arduino UNO R3 Terminals
  +  --> 9 (with/without 100 Ohm resistor)
  -  --> GND
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

// Interval for colon to on and off
int interval = 500;

// Configuration for the buzzer
const int buzzer = 9; //buzzer to arduino pin 9
int beepCount = 0;


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

    // Setup for the buzzer
    pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
 
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

  // Get the number of beeps base on the hour
  if ((dt.Minute() == 0) && (dt.Second() == 0)) {
    if (dt.Hour() > 12){
      beepCount = dt.Hour() - 12;
    } else {
      beepCount = dt.Hour();
    }
  }

    if (dt.Hour() == 0){ // 00:00 --> 12:00AM
      displayTime(12, dt.Minute(), true);
    } else if ((dt.Hour() > 0) && (dt.Hour() < 12) ){ // 01:00-11:00 --> 1:00AM-11:00AM
      displayTime(dt.Hour(), dt.Minute(), true);
    } else if (dt.Hour() == 12){ // 12:00 --> 12:00PM
      displayTime(12, dt.Minute(), false);
    } else { // 13:00-23:00 --> 1:00PM-11:00PM
      displayTime(dt.Hour() - 12, dt.Minute(), false);
    }


    beepCount--;
}

void displayTime(int hour, int minute, bool isAm) {
  seg.displayTime(hour, minute, true, false);
  if (isAm == true) {
    seg.displayExtraLeds(0x06); // upper left point + colon
  } else {
    seg.displayExtraLeds(0x0A); // lower left point + colon
  }

  beepOn();
  delay(interval);

  if (isAm == true) {
    seg.displayExtraLeds(0x04); // upper left point only
  } else {
    seg.displayExtraLeds(0x08); // lower left point only
  }
  
  beepOff();
  delay(interval);
}


void beepOn() {
  if (beepCount > 0) {
    tone(buzzer, 3000); // Send 1KHz sound signal...
  }
}

void beepOff() {
  noTone(buzzer);     // Stop sound...
}

