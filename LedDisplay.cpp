#include "LedDisplay.h"

LedDisplay::LedDisplay()
{
  for (uint8_t row = 0; row < 2; row++) {
    for (uint8_t digit = 0; digit < 4; digit++) {
      if (displayPins[row][digit]) {
        pinMode(displayPins[row][digit], OUTPUT);
        digitalWrite(displayPins[row][digit], HIGH);
      }
    }
  }

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

LedDisplay::displayNumber(uint8_t displayIndex, uint16_t number, uint8_t forcedDigitDisplay)
{
  uint16_t n = number;
  if (displayIndex < 3) {
    uint8_t digit = displayIndex ? 3 : 4;
    while (digit)
    {
      if (n || digit >= (displayIndex ? 2 : 4) || digit >= forcedDigitDisplay + 1) {
        currentData[displayIndex][--digit] = ledEncodedDigits[n % 10];
        n /= 10;
      } else {
        currentData[displayIndex][--digit] = emptyDigit;
      }
    }
  }
}

LedDisplay::displayDot(uint8_t displayIndex, uint16_t digit)
{
  currentData[displayIndex][digit] |= 0b00001000;
}

LedDisplay::clearDot(uint8_t displayIndex, uint16_t digit)
{
  currentData[displayIndex][digit] &= 0b11110111;
}

LedDisplay::setBlinking(uint8_t displayIndex, uint16_t digit)
{
  blinking[displayIndex][digit] = true;
}

LedDisplay::resetBlinking(uint8_t displayIndex, uint16_t digit)
{
  blinking[displayIndex][digit] = false;
}

LedDisplay::update()
{
  digitalWrite(latchPin, LOW);
  digitalWrite(clockPin, LOW);

  if (blinking[digitToUpdateIndex[0]][digitToUpdateIndex[1]] && blinkPeriod)
    shiftOut(dataPin, clockPin, MSBFIRST, emptyDigit);
  else
    shiftOut(dataPin, clockPin, MSBFIRST, currentData[digitToUpdateIndex[0]][digitToUpdateIndex[1]]);

  for (uint8_t row = 0; row < 2; row++) {
    for (uint8_t digit = 0; digit < 4; digit++) {
      if (displayPins[row][digit]) {
        digitalWrite(displayPins[row][digit], HIGH);
      }
    }
  }
  digitalWrite(latchPin, HIGH);
  digitalWrite(displayPins[digitToUpdateIndex[0]][digitToUpdateIndex[1]], LOW);

  digitToUpdateIndex[1]++;

  if (digitToUpdateIndex[0] && digitToUpdateIndex[1] > 2) {
    digitToUpdateIndex[0] = 0;
    digitToUpdateIndex[1] = 0;
  } else if (!digitToUpdateIndex[0] && digitToUpdateIndex[1] > 3) {
    digitToUpdateIndex[0] = 1;
    digitToUpdateIndex[1] = 0;
  }

  periodCounter++;

  if (periodCounter == 100) {
    blinkPeriod = true;
  } else if (periodCounter >= 200) {
    blinkPeriod = false;
    periodCounter = 0;
  }
}
