/*

References:

Adafruit LED Backpacks, Page 39 of 74 (wiring)
https://us.rs-online.com/m/d/b79e41ef35af9ade9f1fced30a55a922.pdf 

Arduino Library for HT16K33 4x7segment display
https://github.com/RobTillaart/HT16K33

Demo Display Time (code)
https://github.com/RobTillaart/HT16K33/blob/master/examples/demo_displayTime/demo_displayTime.ino


How to Wire and Program a Button
https://docs.arduino.cc/built-in-examples/digital/Button


HT16K33 Backpack to Arduino UNO R3 Terminals
  IO    --> Jumper to 5V
  +     --> 5V
  -     --> GND
  D     --> SDA
  C     --> SCL

*/

#include "HT16K33.h"

HT16K33 seg(0x70);

uint8_t hour, minute, second;

// Configuration for the push putton switch
const int buttonPin = 2;
int buttonState = 0;


void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);

  // Setup up the HT16K33 Backpack
  seg.begin();
  Wire.setClock(100000);
  seg.displayOn();
  seg.setDigits(4);

  // Setup the push button switch
  pinMode(buttonPin, OUTPUT);

  // Manual assignment of hour, minute and second
  hour = 23;
  minute = 15;
  second = 0;

}

void loop() {

  // Reads the state of the push button switch
  buttonState = digitalRead(buttonPin);

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
    } else {
      minute++;
    }
  } else {
    second++;
  }

  //  Detect if the button is pressed
  if (buttonState == HIGH){
    seg.displayTime(minute, second, true, true); // if pressed then display minute and seconds
  } else {
    seg.displayTime(hour, minute, true, true); // if not pressed then display hour and minute
  }

  seg.displayColon(1); // activate the colon
  delay(500);
  seg.displayColon(0); // deactivate the colon
  delay(500);
}
