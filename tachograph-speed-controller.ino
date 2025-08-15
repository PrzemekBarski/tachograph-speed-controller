#include "DFRobot_RTU.h"
#include "LedDisplay.h"
#include "SpeedMeter.h"
#include <avr/interrupt.h>

#if !defined(setbit)
    #define setbit(val,b) (  (val) |= (1<<(b)) )
    #define clrbit(val,b) (  (val) &=~(1<<(b)) )
    #define tstbit(val,b) ( ((val) &(1<<(b-8)))!=0 )
#endif
                   
DFRobot_RTU modbus(&Serial1, 2);
// String paramNames[] = { "MB_Current",
//                         "MB_BrakeCurrent",
//                         "MB_Voltage",
//                         "MB_Freq",
//                         "MB_I/O_Stop13",
//                         "MB_I/O_DIR",
//                         "MB_I/O_SPEED",
//                         "MB_I/O_ILIM",
//                         "MB_I/O_DISABLE",
//                         "MB_PWM",
//                         "MB_Speed2Enable" };

LedDisplay display;
SpeedMeter speedMeter(display, modbus);

void setup() {
  // Serial.begin(9600);
  // while (!Serial) ;

  TCCR3A = 0;  // Initialise registers
  TCCR3B = 0;  // Initialise registers
  TCCR3C = 0;  // Initialise registers

  OCR3A = 30;
  OCR3BH = 0;
  OCR3BL = 0;

  setbit(TCCR3B, CS32);
  clrbit(TCCR3B, CS31);
  setbit(TCCR3B, CS30);
  setbit(TCCR3B, WGM32);

  setbit(TIMSK3, OCIE3A);

  Serial1.begin(19200, SERIAL_8E1);

  while (!Serial1) ;

  uint8_t device_id = 0;

  uint8_t status = 20;

  while (status) {
    status = modbus.readHoldingRegister(1, 19, &device_id, 1);
  }

  speedMeter.begin();
}

ISR(TIMER3_COMPA_vect) {
  display.update();
}

void loop() {
  speedMeter.handle();

  // uint16_t ret[10];
  // uint16_t status;

  // status = modbus.readInputRegister(1, 1, &ret, 10);

  // if (!status) {
  //   Serial.println("\n\n");
  //   for(uint8_t ParamID = 0; ParamID < 11; ParamID++) {
  //     Serial.print(paramNames[ParamID]);
  //     Serial.print(": ");
  //     Serial.println(ret[ParamID] >> 8);
  //   }
  // }
}
