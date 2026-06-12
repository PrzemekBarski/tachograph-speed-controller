#ifndef DISPLAY_LEDDISPLAY_H_
#define DISPLAY_LEDDISPLAY_H_

#include "Arduino.h"

class LedDisplay {
public:
  LedDisplay();
  update();
  displayNumber(uint8_t displayIndex, uint16_t number, uint8_t forcedDigitDisplay = 5);
  displayDot(uint8_t displayIndex, uint16_t digit);
  clearDot(uint8_t displayIndex, uint16_t digit);
  setBlinking(uint8_t displayIndex, uint16_t digit);
  resetBlinking(uint8_t displayIndex, uint16_t digit);
  clearDisplay(uint8_t displayIndex);
private:
  uint8_t emptyDigit = 0b00000000;
  uint8_t letter_d = 0b01110111;
  uint8_t letter_i = 0b00000100;
  uint8_t ledEncodedDigits[10] = { 0b01110111,
                                   0b00010100,
                                   0b10110011,
                                   0b10110110,
                                   0b11010100,
                                   0b11100110,
                                   0b11100111,
                                   0b01110100,
                                   0b11110111,
                                   0b11110110 };
  uint8_t displayPins[2][4] = {{ 13, 18, 19, 20 },
                               { 21, 22, 23, 0 }};
  uint8_t dataPin = 5, clockPin = 4, latchPin = 3;
  uint8_t currentData[2][4] = {{ emptyDigit, emptyDigit, emptyDigit, emptyDigit },
                               { emptyDigit, emptyDigit, emptyDigit, 0 }};
  bool blinking[2][4] = {{ false, false, false, false },
                         { false, false, false, false }};
  bool blinkPeriod = false;
  uint8_t periodCounter = 0;
  uint8_t digitToUpdateIndex[2] = {0, 0};
};

#endif /* DISPLAY_LEDDISPLAY_H_ */
