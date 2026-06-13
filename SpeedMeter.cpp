#include "SpeedMeter.h"

String paramNames[] = { "MB_Current",
                        "MB_BrakeCurrent",
                        "MB_Voltage",
                        "MB_Freq",
                        "MB_I/O_Stop13",
                        "MB_I/O_DIR",
                        "MB_I/O_SPEED",
                        "MB_I/O_ILIM",
                        "MB_I/O_DISABLE",
                        "MB_PWM",
                        "MB_Speed2Enable",
                        "unknown",
                        "unknown",
                        "unknown",
                        "unknown" };

SpeedMeter::SpeedMeter(LedDisplay& display, DFRobot_RTU& modbus) :
  display(display), modbus(modbus)
{
  buttonDown = new Button(8);
  buttonAction = new Button(9);
  buttonUp = new Button(10);

  display.displayNumber(1, speed_km_h);
}

void SpeedMeter::begin()
{
  buttonDown->begin();
  buttonAction->begin();
  buttonUp->begin();

  setDriverSettings();
}

void SpeedMeter::actionBtn()
{
  // If the value exceeds the limits, change it to the nearest limit
  if (editingState && editingState < 5) {
    if (temporaryWheelDiameter_mm < minDiameter)
      temporaryWheelDiameter_mm = minDiameter;
    else if (temporaryWheelDiameter_mm > maxDiameter)
      temporaryWheelDiameter_mm = maxDiameter;
  } else if (editingState && editingState < 9) {
    if (temporaryGearboxRatio < 0.3f)
      temporaryGearboxRatio = 0.3f;
    else if (temporaryGearboxRatio > 3.0f)
      temporaryGearboxRatio = 3.0f;
  }

  // Cycle through editing states: 0 - editing disabled; 1-4 - edit wheel diameter; 5-8 - edit gearbox ratio
  editingState = editingState >= 8 ? 0 : editingState + 1;

  // Turn off blinking
  for (uint8_t i = 0; i < 4; i++) {
    display.resetBlinking(0, i);
  }

  if (editingState) {
    if (editingState < 5) {
      display.displayNumber(0, temporaryWheelDiameter_mm, 5 - editingState);
      display.setBlinking(0, editingState - 1);
      display.displayNumber(1, 1);
    } else {
      if (editingState == 5) {
        temporaryGearboxRatio = gearboxRatio;
      }

      floatToDigitArray(temporaryGearboxRatio, temporaryRatioDigitArray, 3);
      display.displayNumber(0, digitArrayToInt(temporaryRatioDigitArray), 4);
      display.displayDot(0, 0);
      display.setBlinking(0, editingState - 5);
      display.displayNumber(1, 2);
    }
  } else {
    wheelDiameter_mm = temporaryWheelDiameter_mm;
    gearboxRatio = temporaryGearboxRatio;

    display.clearDisplay(0);
    setDriverSettings();
  }
}

void SpeedMeter::intToDigitArray(uint16_t number, uint8_t *array)
{
  uint8_t digit = 4;
  uint16_t n = number;

  while (digit)
  {
    array[--digit] = n % 10;
    n /= 10;
  }
}

float SpeedMeter::digitArrayToFloat(uint8_t *array, uint8_t dotPosition)
{
  uint16_t number = 0;

  for (uint8_t digit = 0; digit < 4; digit++)
  {
    number += array[digit] * round(pow(10, 3 - digit));
  }

  if (!dotPosition)
    return float(number);

  return float(number) / pow(10.0f, dotPosition);
}

void SpeedMeter::floatToDigitArray(float value, uint8_t *array, uint8_t dotPosition)
{
  if (dotPosition > 3)
    dotPosition = 0;

  uint16_t integerValue = 0;
  if (dotPosition)
    integerValue = uint16_t(round(value * pow(10.0f, dotPosition)));
  else
    integerValue = uint16_t(round(value));

  uint8_t digit = 4;
  while (digit)
  {
    array[--digit] = integerValue % 10;
    integerValue /= 10;
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

void SpeedMeter::downBtn()
{
  if (!editingState)
    return;

  if (editingState < 5) {
    uint8_t digitArray[4];
    intToDigitArray(temporaryWheelDiameter_mm, digitArray);

    if (digitArray[editingState - 1])
      digitArray[editingState - 1] -= 1;
    else if (editingState == 1)
      digitArray[editingState - 1] = 1;
    else
      digitArray[editingState - 1] = 9;

    temporaryWheelDiameter_mm = digitArrayToInt(digitArray);
    display.displayNumber(0, temporaryWheelDiameter_mm, 5 - editingState);
    display.setBlinking(0, editingState - 1);
  } else {
    uint8_t digitIndex = editingState - 5;

    if (temporaryRatioDigitArray[digitIndex])
      temporaryRatioDigitArray[digitIndex] -= 1;
    else
      temporaryRatioDigitArray[digitIndex] = 9;

    temporaryGearboxRatio = digitArrayToFloat(temporaryRatioDigitArray, 3);

    display.displayNumber(0, digitArrayToInt(temporaryRatioDigitArray), 4);
    display.displayDot(0, 0);
    display.setBlinking(0, digitIndex);
  }
}

void SpeedMeter::upBtn()
{
  if (!editingState)
    return;

  if (editingState < 5) {
    uint8_t digitArray[4];
    intToDigitArray(temporaryWheelDiameter_mm, digitArray);

    if (digitArray[editingState - 1] < (editingState == 1 ? 1 : 9))
      digitArray[editingState - 1] += 1;
    else
      digitArray[editingState - 1] = 0;

    temporaryWheelDiameter_mm = digitArrayToInt(digitArray);
    display.displayNumber(0, temporaryWheelDiameter_mm, 5 - editingState);
    display.setBlinking(0, editingState - 1);
  } else {
    uint8_t digitIndex = editingState - 5;

    if (temporaryRatioDigitArray[digitIndex] < 9)
      temporaryRatioDigitArray[digitIndex] += 1;
    else
      temporaryRatioDigitArray[digitIndex] = 0;

    temporaryGearboxRatio = digitArrayToFloat(temporaryRatioDigitArray, 3);

    display.displayNumber(0, digitArrayToInt(temporaryRatioDigitArray), 4);
    display.displayDot(0, 0);
    display.setBlinking(0, digitIndex);
  }
}

void SpeedMeter::handle()
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

  if (currentTime >= lastUpdateTime + 200) {
    lastUpdateTime = currentTime;

    if (!editingState) {
      uint16_t params[10];
      uint16_t status;
      status = modbus.readInputRegister(1, 1, &params, 10);

      if (!status) {
        // Serial.println("\n\n");
        // for(uint8_t ParamID = 0; ParamID < 13; ParamID++) {
        //   Serial.print(ParamID);
        //   Serial.print(". ");
        //   Serial.print(paramNames[ParamID]);
        //   Serial.print(": ");
        //   Serial.println(params[ParamID] >> 8);
        // }

        // Serial.println(float(params[3]) / float(params[3] >> 8));

        uint8_t supplyVoltage10 = params[2] >> 8;
        voltageMultiplier = 240 / supplyVoltage10;

        uint8_t rps10 = params[3] >> 8;
        
        // Filter out suspicious values
        if ((rps10 < previousRPS) && lowValueCounter < 14 ) {
          rps10 = previousRPS;
          lowValueCounter++;
        } else {
          lowValueCounter = 0;
          previousRPS = rps10;
        }
        float rpm = (float(rps10) * 60) / 11.f;
        float circumference = PI * wheelDiameter_mm;
        float speed10 = circumference * rpm * 60 / gearboxRatio / 100000.f;
        
        if (speed10 < 1000) {
          speed_km_h = round(speed10);
          display.displayNumber(1, speed_km_h, 2);
          display.displayDot(1, 1);
        } else {
          speed_km_h = round(speed10 / 10);
          display.displayNumber(1, speed_km_h, 2);
          display.clearDot(1, 1);
        }
      }
    } else {
      // display.clearDisplay(1);
    }

  }
}

void SpeedMeter::setDriverSettings()
{
  uint8_t status[4] = {20, 20, 20, 20};
  uint8_t startRange = maxMotorStart - minMotorStart;
  uint8_t maxMotorGainLimited = min((maxMotorGain * gearboxRatio), 75);
  float gainRange = (maxMotorGainLimited - (minMotorGain * gearboxRatio)) * voltageMultiplier;
  uint16_t diameterRange = maxDiameter - minDiameter;
  float ratio = float(wheelDiameter_mm - minDiameter) / float(diameterRange);
  uint8_t start = maxMotorStart - round(ratio * startRange);
  uint8_t gain = round(maxMotorGainLimited - (ratio * gainRange));
  uint8_t timeout = 10;
  
  while ((status[0] || status[1]) && timeout) {
    status[0] = modbus.writeHoldingRegister(1, 26, start); // Start speed

    delay(2);

    if (!status[0]) {
      status[1] = modbus.writeHoldingRegister(1, 20, 1);
    }

    timeout--;
  }

  delay(2);
  timeout = 10;

  while ((status[2] || status[3]) && timeout) {
    status[2] = modbus.writeHoldingRegister(1, 27, gain); // Speed limit

    delay(2);

    if (!status[2]) {
      status[3] = modbus.writeHoldingRegister(1, 20, 1);
    }

    timeout--;
  }

}