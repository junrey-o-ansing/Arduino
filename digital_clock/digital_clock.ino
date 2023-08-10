//
//    FILE: demo_displayTime.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: http://www.adafruit.com/products/1002
//     URL: https://github.com/RobTillaart/HT16K33

// sketch will work correctly up to 99:99

#include "HT16K33.h"

HT16K33 seg(0x70);

uint8_t hour, minute, second;

const int buttonPin = 2;
int buttonState = 0;


void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);

  seg.begin();
  Wire.setClock(100000);
  seg.displayOn();
  seg.setDigits(4);
  hour = 23;
  minute = 15;
  second = 0;

  pinMode(buttonPin, OUTPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (second == 59){
    second = 0;
    if (minute == 59){
      minute = 0;
      if (hour == 23){
        hour = 0;
      } else {
        hour++;
      }
    } else {
      minute++;
    }
  } else {
    second++;
  }

  if (buttonState == HIGH){
    seg.displayTime(minute, second, true, true);  
  } else {
    seg.displayTime(hour, minute, true, true);
  }
  // seg.displayTime(minute, second, true, true);
  seg.displayColon(1);
  delay(500);
  seg.displayColon(0);
  delay(500);
}
