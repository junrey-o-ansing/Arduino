/*

References:

Adafruit LED Backpacks, Page 39 of 74 (wiring)
https://us.rs-online.com/m/d/b79e41ef35af9ade9f1fced30a55a922.pdf 

Arduino Library for HT16K33 4x7segment display
https://github.com/RobTillaart/HT16K33

Demo Display Time (code)
https://github.com/RobTillaart/HT16K33/blob/master/examples/demo_displayTime/demo_displayTime.ino

HT16K33 Backpack to Arduino UNO R3 Terminals
  IO    --> Jumper to 5V
  +     --> 5V
  -     --> GND
  D     --> SDA
  C     --> SCL

*/

#include "HT16K33.h"

HT16K33 seg(0x70);

// Manual input of initial hour, minute and seconds
int hour = 9;
int minute = 59;
int second = 50;

// Set the inteval to 500 ms or .5 sec
int interval = 500;

const int buzzer = 9; //buzzer to arduino pin 9
int beepCount = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);

  seg.begin();
  Wire.setClock(100000);
  seg.displayOn();
  seg.setDigits(4);

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
}

void loop() {

  // The logic for the time based on a 24-hour format
  if (second == 59){
    second = 0;
    if (minute == 59){
      minute = 0;
      if (hour == 23){
        hour = 0;
      } else {
        hour++;
      }

      // if (hour > 12){
      //   beepCount = hour - 12;
      // } else {
      //   beepCount = hour;
      // }
    } else {
      minute++;
    }
  } else {
    second++;
  }

  // Get the number of beeps base on the hour
  if ((minute == 0) && (second == 0)) {
    if (hour > 12){
      beepCount = hour - 12;
    } else {
      beepCount = hour;
    }
  }

  // Translate the 24-hour to 12-hour format
  // Use the upper left point to indicate AM
  // Use the lower left point to indicate PM
  if (hour == 0){ // 00:00 --> 12:00AM
    seg.displayTime(12, minute, true, false);
    seg.displayExtraLeds(0x06); // upper left point + colon
    beepOn();
    delay(interval);
    seg.displayExtraLeds(0x04); // upper left point only
    beepOff();
    delay(interval);
  } else if ((hour > 0) && (hour < 12) ){ // 01:00-11:00 --> 1:00AM-11:00AM
    seg.displayTime(hour, minute, true, false);
    seg.displayExtraLeds(0x06); // upper left point + colon
    beepOn();
    delay(interval);
    seg.displayExtraLeds(0x04); // upper left point only
    beepOff();
    delay(interval);
  } else if (hour == 12){ // 12:00 --> 12:00PM
    seg.displayTime(12, minute, true, false);
    seg.displayExtraLeds(0x0A); // lower left point + colon
    beepOn();
    delay(interval);
    seg.displayExtraLeds(0x08); // lower left point only
    beepOff();
    delay(interval);
  } else { // 13:00-23:00 --> 1:00PM-11:00PM
    seg.displayTime(hour - 12, minute, true, false);
    seg.displayExtraLeds(0x0A); // lower left point + colon
    beepOn();
    delay(interval);
    seg.displayExtraLeds(0x08); // lower left point only
    beepOff();
    delay(interval);
  }

  beepCount--;
}

void beepOn() {
  if (beepCount > 0) {
    tone(buzzer, 3000); // Send 1KHz sound signal...
  }
}

void beepOff() {
  noTone(buzzer);     // Stop sound...
}

/* displayExtraLeds() pattern based on mask

0x00/0x01 - all off
0x02/0x03 - colon
0x04/0x05 - upper left point
0x06/0x07 - upper left point + colon
0x08/0x09 - lower left point
0x0A/0x0B - lower left point + colon 
0x0C/0x0D - upper left point + lower left point
0x0E/0x0F - upper left point + lower left point + colon
0x10/0x11 - upper point
0x12/0x13 - upper point + colon
0x14/0x15 - upper point + upper left point
0x16/0x17 - upper point + upper left point + colon
0x18/0x19 - upper point + lower left point

*/