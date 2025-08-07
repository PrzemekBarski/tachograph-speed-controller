#include "SpeedMeter.h"

SpeedMeter::SpeedMeter(LedDisplay& display, DFRobot_RTU& modbus) :
  display(display), modbus(modbus)
{
  buttonDown = new Button(8);
  buttonAction = new Button(9);
  buttonUp = new Button(10);

  display.displayNumber(0, wheelDiameter_mm);
  display.displayNumber(1, speed_km_h);
}

SpeedMeter::begin()
{
  buttonDown->begin();
  buttonAction->begin();
  buttonUp->begin();

  setDriverSettings();
}

SpeedMeter::actionBtn()
{
  editingState = editingState > 3 ? 0 : editingState + 1;

  for (uint8_t i = 0; i < 4; i++) {
    display.resetBlinking(0, i);
  }

  if (editingState) {
    display.displayNumber(0, temporaryWheelDiameter_mm, editingState - 1);
    display.setBlinking(0, editingState - 1);
  } else {
    if (temporaryWheelDiameter_mm < minDiameter)
      wheelDiameter_mm = minDiameter;
    else if (temporaryWheelDiameter_mm > maxDiameter)
      wheelDiameter_mm = maxDiameter;
    else
      wheelDiameter_mm = temporaryWheelDiameter_mm;

    temporaryWheelDiameter_mm = wheelDiameter_mm;

    display.displayNumber(0, wheelDiameter_mm);
    setDriverSettings();
  }
}

SpeedMeter::intToDigitArray(uint16_t number, uint8_t *array)
{
  uint8_t digit = 4;
  uint16_t n = number;

  while (digit)
  {
    array[--digit] = n % 10;
    n /= 10;
  }
}

uint16_t SpeedMeter::digitArrayToInt(uint8_t *array)
{
  uint16_t number = 0;

  for (uint8_t digit = 0; digit < 4; digit++)
  {
    number += array[digit] * round(pow(10, 3 - digit));
  }

  return number;
}

SpeedMeter::downBtn()
{
  uint8_t digitArray[4];

  intToDigitArray(temporaryWheelDiameter_mm, digitArray);

  if (editingState) {
    if (digitArray[editingState - 1])
      digitArray[editingState - 1] -= 1;
    else if (editingState == 1)
      digitArray[editingState - 1] = 1;
    else
      digitArray[editingState - 1] = 9;
    
    temporaryWheelDiameter_mm = digitArrayToInt(digitArray);

    display.displayNumber(0, temporaryWheelDiameter_mm, editingState - 1);
    display.setBlinking(0, editingState - 1);
  }
}

SpeedMeter::upBtn()
{
  uint8_t digitArray[4];

  intToDigitArray(temporaryWheelDiameter_mm, digitArray);

  if (editingState) {
    if (digitArray[editingState - 1] < (editingState == 1 ? 1 : 9))
      digitArray[editingState - 1] += 1;
    else
      digitArray[editingState - 1] = 0;
    
    temporaryWheelDiameter_mm = digitArrayToInt(digitArray);

    display.displayNumber(0, temporaryWheelDiameter_mm, editingState - 1);
    display.setBlinking(0, editingState - 1);
  }
}

SpeedMeter::handle()
{
  unsigned long currentTime = millis();

  buttonDown->handle();
  buttonAction->handle();
  buttonUp->handle();

  if (buttonDown->resetClicked())
    downBtn();

  if (buttonAction->resetClicked())
    actionBtn();

  if (buttonUp->resetClicked())
    upBtn();

  if (currentTime >= lastUpdateTime + 400) {
    lastUpdateTime = currentTime;

    uint8_t rps10 = modbus.readInputRegister(1, 4);
    uint16_t rpm = rps10 * 6;
    float circumference = PI * wheelDiameter_mm;
    float speed10 = circumference * rpm * 60 / 100000.f;
    
    if (speed10 < 1000) {
      speed_km_h = round(speed10);
      display.displayNumber(1, speed_km_h);
      display.displayDot(1, 1);
    } else {
      speed_km_h = round(speed10 / 10);
      display.displayNumber(1, speed_km_h);
      display.clearDot(1, 1);
    }

  }
}

SpeedMeter::setDriverSettings()
{
  uint8_t status[4] = {20, 20, 20, 20};
  uint8_t startRange = maxMotorStart - minMotorStart;
  uint8_t gainRange = maxMotorGain - minMotorGain;
  uint16_t diameterRange = maxDiameter - minDiameter;
  float ratio = float(wheelDiameter_mm - minDiameter) / float(diameterRange);
  uint8_t start = maxMotorStart - round(ratio * startRange);
  uint8_t gain = maxMotorGain - round(ratio * gainRange);
  
  while (status[0] || status[1]) {
    status[0] = modbus.writeHoldingRegister(1, 26, start); // Start speed

    delay(2);

    if (!status[0]) {
      status[1] = modbus.writeHoldingRegister(1, 20, 1);
    }
  }

  delay(2);

  while (status[2] || status[3]) {
    status[2] = modbus.writeHoldingRegister(1, 27, gain); // Speed limit

    delay(2);

    if (!status[2]) {
      status[3] = modbus.writeHoldingRegister(1, 20, 1);
    }
  }

}