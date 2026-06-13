#ifndef SPEEDMETER_H_
#define SPEEDMETER_H_

#include "Arduino.h"
#include "LedDisplay.h"
#include "DFRobot_RTU.h"
#include <AbleButtons.h>

using Button = AblePullupClickerButton;

class SpeedMeter {
public:
  SpeedMeter(LedDisplay& display, DFRobot_RTU& modbus);
  void begin();
  void handle();
  void downBtn();
  void actionBtn();
  void upBtn();
  void setDriverSettings();
  void intToDigitArray(uint16_t number, uint8_t *array);
  uint16_t digitArrayToInt(uint8_t *array);
  float digitArrayToFloat(uint8_t *array, uint8_t dotPosition);
private:
  void floatToDigitArray(float value, uint8_t *array, uint8_t dotPosition);

  LedDisplay &display;
  DFRobot_RTU &modbus;
  float voltageMultiplier = 1.0f;
  uint16_t speed_km_h = 0;
  uint16_t wheelDiameter_mm = 830, minDiameter = 700, maxDiameter = 1600;
  float gearboxRatio = 1.0f;
  uint8_t minMotorGain = 15, maxMotorGain = 37, minMotorStart = 20, maxMotorStart = 20;
  uint8_t previousRPS = 0, lowValueCounter = 0;
  uint16_t temporaryWheelDiameter_mm = wheelDiameter_mm;
  float temporaryGearboxRatio = gearboxRatio;
  uint8_t temporaryRatioDigitArray[4] = {0, 0, 0, 0};
  unsigned long lastUpdateTime = 0;
  uint16_t editingState = 0;
  bool firstRun = true;
  Button *buttonDown, *buttonAction, *buttonUp;
};

#endif /* SPEEDMETER_H_ */
