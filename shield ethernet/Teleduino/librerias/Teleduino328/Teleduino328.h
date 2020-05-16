/*
  Teleduino328.h - Teleduino328 library
  Version 328-0.6.9
  Nathan Kennedy 2009 - 2014
  http://www.teleduino.org

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef Teleduino328_h
#define Teleduino328_h

#define TD328_PINS 22
#define TD328_SERVO_INSTANCES 6
#define TD328_SHIFT_REGISTERS 2
#define TD328_SHIFT_REGISTER_TIMERS 8

#include <Arduino.h>
#include <EEPROM.h>
#include <Servo.h>
#include <Wire.h>

// Used for Teleduino328Class::getFreeMemory()
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

typedef struct
{
  byte mode;
  byte expireAction;
  unsigned long expireTime;
} Pin;

typedef struct
{
  Servo servo;
  byte position;
} ServoInstance;

typedef struct
{
  byte clockPin;
  byte dataPin;
  byte enablePin;
  byte latchPin;
  byte outputs[32];
} ShiftRegister;

typedef struct
{
  byte shiftRegister;
  byte action;
  unsigned long expireTime;
  byte outputs[32];
} ShiftRegisterTimer;

class Teleduino328Class
{
  protected:
    boolean _reset;
    Pin _pins[TD328_PINS];
    ServoInstance _servoInstances[TD328_SERVO_INSTANCES];
    ShiftRegister _shiftRegisters[TD328_SHIFT_REGISTERS];
    ShiftRegisterTimer _shiftRegisterTimers[TD328_SHIFT_REGISTER_TIMERS];
    byte _statusLedPin;
    char _version[17];
  boolean _wireDefined;
  public:
    Teleduino328Class();
    boolean checkAnalogPin(byte);
    boolean checkDigitalPin(byte);
    boolean checkPwmPin(byte);
    void checkReset();
    boolean definePinMode(byte, byte);
    int getFreeMemory();
    byte hexDecode(byte);
    byte hexEncode(byte, boolean);
    byte hexEncode(byte);
    void instruction(byte*);
    void loadPresets();
    void pinTimers();
    void reset();
    boolean setDigitalOutput(byte, byte, unsigned int, byte);
    boolean setDigitalOutput(byte, byte, unsigned int);
    boolean setDigitalOutput(byte, byte);
    void setStatusLed(unsigned int, boolean, long);
    void setStatusLed(unsigned int, boolean);
    void setStatusLed(unsigned int, long);
    void setStatusLed(unsigned int);
    boolean setStatusLedPin(byte);
    void shiftRegisters();
    void shiftRegisterTimers();
};

extern Teleduino328Class Teleduino328;

#endif
