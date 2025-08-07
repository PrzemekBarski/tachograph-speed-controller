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
  handle();
  downBtn();
  actionBtn();
  upBtn();
  begin();
  setDriverSettings();
  intToDigitArray(uint16_t number, uint8_t *array);
  uint16_t digitArrayToInt(uint8_t *array);
private:
  LedDisplay &display;
  DFRobot_RTU &modbus;
  uint16_t speed_km_h = 0;
  uint16_t wheelDiameter_mm = 830, minDiameter = 700, maxDiameter = 1600;
  uint8_t minMotorGain = 30, maxMotorGain = 75, minMotorStart = 20, maxMotorStart = 20;
  uint16_t temporaryWheelDiameter_mm = wheelDiameter_mm;
  unsigned long lastUpdateTime = 0;
  uint16_t editingState = 0;
  bool firstRun = true;
  Button *buttonDown, *buttonAction, *buttonUp;
};

#endif /* SPEEDMETER_H_ */
