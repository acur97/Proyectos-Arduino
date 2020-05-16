/*
  Teleduino328.cpp - Teleduino328 library
  Version 328-0.6.9
  Nathan Kennedy 2009 - 2014
  http://www.teleduino.org

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Teleduino328.h"

Teleduino328Class::Teleduino328Class()
{
  _reset = false;
  for(byte i = 0; i < 100; i++)
  {
    if(i < TD328_PINS)
    {
      _pins[i].mode = 255;
      _pins[i].expireAction = 255;
      _pins[i].expireTime = 0;
    }
    if(i < TD328_SERVO_INSTANCES)
    {
      _servoInstances[i].position = 255;
    }
    if(i < TD328_SHIFT_REGISTERS)
    {
      _shiftRegisters[i].clockPin = 255;
      _shiftRegisters[i].dataPin = 255;
      _shiftRegisters[i].enablePin = 255;
      _shiftRegisters[i].latchPin = 255;
      for(byte j = 0; j < 32; j++)
      {
        _shiftRegisters[i].outputs[j] = 0;
      }
    }
    if(i < TD328_SHIFT_REGISTER_TIMERS)
    {
      _shiftRegisterTimers[i].shiftRegister = 255;
      _shiftRegisterTimers[i].action = 255;
      _shiftRegisterTimers[i].expireTime = 0;
      for(byte j = 0; j < 32; j++)
      {
        _shiftRegisterTimers[i].outputs[j] = 0;
      }
    }
  }
  _statusLedPin = 255;
  strcpy(_version, "328-0.6.9");
  _wireDefined = false;
}

boolean Teleduino328Class::checkAnalogPin(byte pin)
{
  if(pin >= 14 && pin <= 21)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean Teleduino328Class::checkDigitalPin(byte pin)
{
  if(pin >= 0 && pin <= 21)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean Teleduino328Class::checkPwmPin(byte pin)
{
  if(pin == 3 || (pin >= 5 && pin <= 6) || (pin >= 9 && pin <= 11))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Teleduino328Class::checkReset()
{
  if(_reset)
  {
    reset();
  }
}

boolean Teleduino328Class::definePinMode(byte pin, byte mode)
{
  if(checkDigitalPin(pin))
  {
    _pins[pin].mode = mode;
    pinMode(pin, mode);
    return true;
  }
  else
  {
    return false;
  }
}

int Teleduino328Class::getFreeMemory()
{
  int freeMemory;
  if((int)__brkval == 0)
  {
    freeMemory = ((int)&freeMemory) - ((int)&__bss_end);
  }
  else
  {
    freeMemory = ((int)&freeMemory) - ((int)__brkval);
  }
  return freeMemory;
}

byte Teleduino328Class::hexDecode(byte c)
{
  if(c >= '0' && c <= '9')
  {
    return c - '0';
  }
  else if(c >= 'a' && c <= 'f')
  {
    return c - 'a' + 10;
  }
  else if(c >= 'A' && c <= 'F')
  {
    return c - 'A' + 10;
  }
  else
  {
    return 0;
  }
}

byte Teleduino328Class::hexEncode(byte n, boolean cap)
{
  if(n >= 0 && n <= 9)
  {
    return n + '0';
  }
  else if(n >= 10 && n <= 15)
  {
    if(cap)
    {
      return n - 10 + 'A';
    }
    else
    {
      return n - 10 + 'a';
    }
  }
  else
  {
    return '0';
  }
}

byte Teleduino328Class::hexEncode(byte n)
{
  return hexEncode(n, true);
}

void Teleduino328Class::instruction(byte* buffer)
{
  switch(buffer[0])
  {
    case 0x10: // reset
      if(buffer[1] == 0)
      {
        _reset = true;
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x11: // getVersion
      if(buffer[1] == 0)
      {
        buffer[0] = 1;
        byte length;
        for(length = 0; length < 16 && _version[length] > 0; length++)
        {
          buffer[2 + length] = _version[length];
        }
        buffer[1] = length;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x12: // setStatusLedPin
      if(buffer[1] == 1 &&
      setStatusLedPin(buffer[2]))
      {
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x13: // setStatusLed
      if(buffer[1] == 1)
      {
        setStatusLed((unsigned int)buffer[2]);
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x14: // getFreeMemory
      if(buffer[1] == 0)
      {
        int value = getFreeMemory();
        buffer[0] = 1;
        buffer[1] = 2;
        buffer[2] = value / 256;
        buffer[3] = value % 256;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x15: // ping
      if(buffer[1] == 0)
      {
        if(_statusLedPin > -1)
        {
          setDigitalOutput(_statusLedPin, HIGH);
          delay(10);
          setDigitalOutput(_statusLedPin, LOW);
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x16: // getUptime
      if(buffer[1] == 0)
      {
        unsigned long value = millis();
        buffer[0] = 1;
        buffer[1] = 4;
        buffer[2] = value / 16777216;
        buffer[3] = (value % 16777216) / 65536;
        buffer[4] = ((value % 16777216) % 65536) / 256;
        buffer[5] = ((value % 16777216) % 65536) % 256;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x17: // loadPresets
      if(buffer[1] == 0)
      {
        loadPresets();
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x20: // definePinMode
      if(buffer[1] == 2 &&
      checkDigitalPin(buffer[2]) &&
      buffer[3] >= 0 && buffer[3] <= 1)
      {
        definePinMode(buffer[2], buffer[3]);
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x21: // setDigitalOutput
      if((buffer[1] == 2 || buffer[1] == 5 || buffer[1] == 6) &&
      checkDigitalPin(buffer[2]) &&
      buffer[3] >= 0 && buffer[3] <= 2)
      {
        unsigned int time = 0;
        byte save = 0;
        if(buffer[1] == 5 || buffer[1] == 6)
        {
          time = ((unsigned long)buffer[4] * 65536) + ((unsigned long)buffer[5] * 256) + (unsigned long)buffer[6];
          if(buffer[1] == 6 && buffer[7] == 1)
          {
            save = 1;
          }
        }
        if(buffer[3] == 2)
        {
          buffer[0] = setDigitalOutput(buffer[2], !digitalRead(buffer[2]), time, save);
        }
        else
        {
          buffer[0] = setDigitalOutput(buffer[2], buffer[3], time, save);
        }
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x22: // setPwmOutput
      if(buffer[1] == 2 &&
      checkPwmPin(buffer[2]))
      {
        analogWrite(buffer[2], buffer[3]);
        _pins[buffer[2]].expireAction = 255;
        _pins[buffer[2]].expireTime = 0;
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x23: // getDigitalInput
      if(buffer[1] == 1 &&
      checkDigitalPin(buffer[2]))
      {
        buffer[0] = 1;
        buffer[1] = 1;
        buffer[2] = digitalRead(buffer[2]);
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x24: // getAnalogInput
      if(buffer[1] == 1 &&
      checkAnalogPin(buffer[2]))
      {
        analogRead(buffer[2] - 14);
        delay(10);
        int value = analogRead(buffer[2] - 14);
        buffer[0] = 1;
        buffer[1] = 2;
        buffer[2] = value / 256;
        buffer[3] = value % 256;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x25: // getAllInputs
      if(buffer[1] == 0)
      {
        buffer[0] = 1;
        buffer[1] = 30;
        for(byte i = 0; i < 14; i++)
        {
          buffer[2 + i] = digitalRead(i);
        }
        for(byte i = 0; i < 8; i++)
        {
          analogRead(i);
          delay(10);
          int value = analogRead(i);
          buffer[16 + (i * 2)] = value / 256;
          buffer[16 + (i * 2) + 1] = value % 256;
        }
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x26: // setDigitalOutputs
      if(buffer[1] >= 5 && !((buffer[1] - 1) % 4) &&
      buffer[2] < TD328_PINS)
      {
        byte max = (buffer[1] - 1) / 4 < TD328_PINS - buffer[2] ? (buffer[1] - 1) / 4 : TD328_PINS - buffer[2];
        for(byte i = 0; i < max; i++)
        {
          if(buffer[(i * 4) + 3] <= 2)
          {
            unsigned long time = ((unsigned long)buffer[(i * 4) + 4] * 65536) + ((unsigned long)buffer[(i * 4) + 5] * 256) + (unsigned long)buffer[(i * 4) + 6];
            if(buffer[(i * 4) + 3] == 2)
            {
              setDigitalOutput(buffer[2] + i, !digitalRead(buffer[2] + i), time);
            }
            else
            {
              setDigitalOutput(buffer[2] + i, buffer[(i * 4) + 3], time);
            }
          }
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x30: // defineShiftRegister
      if(buffer[2] >= 0 && buffer[2] < TD328_SHIFT_REGISTERS &&
      checkDigitalPin(buffer[3]) &&
      checkDigitalPin(buffer[4]) &&
      checkDigitalPin(buffer[5]) &&
      (buffer[1] == 4 || (buffer[1] == 5 && checkDigitalPin(buffer[6]))))
      {
        _shiftRegisters[buffer[2]].clockPin = buffer[3];
        definePinMode(_shiftRegisters[buffer[2]].clockPin, OUTPUT);
        _shiftRegisters[buffer[2]].dataPin = buffer[4];
        definePinMode(_shiftRegisters[buffer[2]].dataPin, OUTPUT);
        _shiftRegisters[buffer[2]].latchPin = buffer[5];
        definePinMode(_shiftRegisters[buffer[2]].latchPin, OUTPUT);
        if(buffer[1] == 5)
        {
          _shiftRegisters[buffer[2]].enablePin = buffer[6];
          definePinMode(_shiftRegisters[buffer[2]].enablePin, OUTPUT);
        }
        for(byte i = 0; i < 32; i++)
        {
          _shiftRegisters[buffer[2]].outputs[i] = 0;
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x31: // setShiftRegister
      if(buffer[1] >= 2 && buffer[1] <= 33 &&
      buffer[2] >= 0 && buffer[2] < TD328_SHIFT_REGISTERS &&
      checkDigitalPin(_shiftRegisters[buffer[2]].clockPin) &&
      checkDigitalPin(_shiftRegisters[buffer[2]].dataPin) &&
      checkDigitalPin(_shiftRegisters[buffer[2]].latchPin))
      {
        for(byte i = 0; i < 32; i++)
        {
          _shiftRegisters[buffer[2]].outputs[i] = (i < (buffer[1] - 1)) ? buffer[3 + i] : 0;
        }
        for(byte i = 0; i < TD328_SHIFT_REGISTER_TIMERS; i++)
        {
          if(_shiftRegisterTimers[i].shiftRegister == buffer[2])
          {
            _shiftRegisterTimers[i].shiftRegister = 255;
            _shiftRegisterTimers[i].action = 255;
            _shiftRegisterTimers[i].expireTime = 0;
            for(byte j = 0; j < 32; j++)
            {
              _shiftRegisterTimers[i].outputs[j] = 0;
            }
          }
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x32: // mergeShiftRegister
      if(buffer[1] >= 6 && buffer[1] <= 37 &&
      buffer[2] >= 0 && buffer[2] < TD328_SHIFT_REGISTERS &&
      buffer[3] >= 0 && buffer[3] <= 2 &&
      checkDigitalPin(_shiftRegisters[buffer[2]].clockPin) &&
      checkDigitalPin(_shiftRegisters[buffer[2]].dataPin) &&
      checkDigitalPin(_shiftRegisters[buffer[2]].latchPin))
      {
        boolean proceed = true;
        unsigned long time = ((unsigned long)buffer[4] * 65536) + ((unsigned long)buffer[5] * 256) + (unsigned long)buffer[6];
        byte outputs[32];
        for(byte i = 0; i < 32; i++)
        {
          outputs[i] = (i < (buffer[1] - 5)) ? buffer[7 + i] : 0;
        }
        if(time > 0)
        {
          proceed = false;
          for(byte i = 0; i < TD328_SHIFT_REGISTER_TIMERS; i++)
          {
            if(_shiftRegisterTimers[i].shiftRegister == 255)
            {
              proceed = true;
            }
          }
        }
        if(proceed)
        {
          for(byte i = 0; i < 32; i++)
          {
            for(byte j = 0; j < 8; j++)
            {
              byte currentBit = 1;
              currentBit = currentBit << j;
              if((outputs[i] & currentBit) > 0)
              {
                switch(buffer[3])
                {
                  case 0:
                    if((_shiftRegisters[buffer[2]].outputs[i] & currentBit) > 0)
                    {
                      _shiftRegisters[buffer[2]].outputs[i] = _shiftRegisters[buffer[2]].outputs[i] - currentBit;
                    }
                    break;
                  case 1:
                    if((_shiftRegisters[buffer[2]].outputs[i] & currentBit) == 0)
                    {
                      _shiftRegisters[buffer[2]].outputs[i] = _shiftRegisters[buffer[2]].outputs[i] + currentBit;
                    }
                    break;
                  case 2:
                    if((_shiftRegisters[buffer[2]].outputs[i] & currentBit) > 0)
                    {
                      _shiftRegisters[buffer[2]].outputs[i] = _shiftRegisters[buffer[2]].outputs[i] - currentBit;
                    }
                    else if((_shiftRegisters[buffer[2]].outputs[i] & currentBit) == 0)
                    {
                      _shiftRegisters[buffer[2]].outputs[i] = _shiftRegisters[buffer[2]].outputs[i] + currentBit;
                    }
                    break;
                }
              }
            }
            for(byte j = 0; j < TD328_SHIFT_REGISTER_TIMERS; j++)
            {
              if(_shiftRegisterTimers[j].shiftRegister == buffer[2])
              {
                _shiftRegisterTimers[j].outputs[i] = _shiftRegisterTimers[j].outputs[i] - (_shiftRegisterTimers[j].outputs[i] & outputs[i]);
              }
            }
          }
          if(time > 0)
          {
            unsigned long expireTime = millis() + time;
            if(expireTime < 1)
            {
              expireTime = 1;
            }
            for(byte i = 0; i < TD328_SHIFT_REGISTER_TIMERS; i++)
            {
              if(_shiftRegisterTimers[i].shiftRegister == 255)
              {
                _shiftRegisterTimers[i].shiftRegister = buffer[2];
                switch(buffer[3])
                {
                  case 0:
                    _shiftRegisterTimers[i].action = 1;
                    break;
                  case 1:
                    _shiftRegisterTimers[i].action = 0;
                    break;
                  case 2:
                    _shiftRegisterTimers[i].action = 2;
                    break;
                }
                _shiftRegisterTimers[i].expireTime = expireTime;
                for(byte j = 0; j < 32; j++)
                {
                  _shiftRegisterTimers[i].outputs[j] = outputs[j];
                }
                break;
              }
            }
          }
          buffer[0] = 1;
          buffer[1] = 0;
        }
        else
        {
          buffer[0] = 0;
          buffer[1] = 0;
        }
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x33: // getShiftRegister
      if(buffer[1] == 1 &&
      buffer[2] >= 0 && buffer[2] < TD328_SHIFT_REGISTERS)
      {
        byte shiftRegister = buffer[2];
        buffer[0] = 1;
        buffer[1] = 32;
        for(byte i = 0; i < 32; i++)
        {
          buffer[2 + i] = _shiftRegisters[shiftRegister].outputs[i];
        }
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x40: // defineSerial
      if(buffer[1] == 2 &&
      buffer[2] == 0 &&
      buffer[3] >= 0 && buffer[3] <= 10)
      {
        switch(buffer[3])
        {
          case 0:
            Serial.begin(300);
            break;
          case 1:
            Serial.begin(1200);
            break;
          case 2:
            Serial.begin(2400);
            break;
          case 3:
            Serial.begin(4800);
            break;
          case 4:
            Serial.begin(9600);
            break;
          case 5:
            Serial.begin(14400);
            break;
          case 6:
            Serial.begin(19200);
            break;
          case 7:
            Serial.begin(28800);
            break;
          case 8:
            Serial.begin(38400);
            break;
          case 9:
            Serial.begin(57600);
            break;
          case 10:
            Serial.begin(115200);
            break;
        }
        Serial.flush();
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x41: // setSerial
      if(buffer[1] >= 2 &&
      buffer[2] == 0)
      {
        for(byte i = 0; i < (buffer[1] - 1); i++)
        {
          Serial.write(buffer[3 + i]);
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x42: // getSerial
      if(buffer[1] == 2 &&
      buffer[2] == 0 &&
      buffer[3] >= 1)
      {
        byte requestedLength = buffer[3];
        byte length;
        buffer[0] = 1;
        for(length = 0; length < requestedLength && Serial.available(); length++)
        {
          buffer[2 + length] = Serial.read();
        }
        buffer[1] = length;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x43: // flushSerial
      if(buffer[1] == 1 &&
      buffer[2] == 0)
      {
        while(Serial.available())
        {
          Serial.read();
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x50: // defineServo
      if(buffer[1] == 2 &&
      buffer[2] >= 0 && buffer[2] < TD328_SERVO_INSTANCES &&
      checkDigitalPin(buffer[3]))
      {
        buffer[0] = 1;
        buffer[1] = 0;
        _servoInstances[buffer[2]].servo.attach(buffer[3]);
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x51: // setServo
      if(buffer[1] == 2 &&
      buffer[2] >= 0 && buffer[2] < TD328_SERVO_INSTANCES &&
      buffer[3] >= 0 && buffer[3] <= 180 &&
      _servoInstances[buffer[2]].servo.attached())
      {
        buffer[0] = 1;
        buffer[1] = 0;
        _servoInstances[buffer[2]].position = buffer[3];
        _servoInstances[buffer[2]].servo.write(buffer[3]);
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x52: // getServo
      if(buffer[1] == 1 &&
      buffer[2] >= 0 && buffer[2] < TD328_SERVO_INSTANCES &&
      _servoInstances[buffer[2]].servo.attached())
      {
        buffer[0] = 1;
        buffer[1] = 1;
        buffer[2] = _servoInstances[buffer[2]].position;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x60: // resetEeprom
      if(buffer[1] == 0)
      {
        buffer[0] = 1;
        buffer[1] = 0;
        for(int i = 0; i < 1024; i++)
        {
          if(EEPROM.read(i) != 255)
          {
            EEPROM.write(i, 255);
          }
        }
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x61: // setEeprom
      if(buffer[1] >= 3 &&
      buffer[2] <= 3)
      {
        int offset = (buffer[2] * 256) + buffer[3];
        for(byte i = 0; i < (buffer[1] - 2); i++)
        {
          if(EEPROM.read(offset + i) != buffer[4 + i])
          {
            EEPROM.write(offset + i, buffer[4 + i]);
          }
        }
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x62: // getEeprom
      if(buffer[1] == 3 &&
      buffer[2] <= 3 &&
      buffer[4] >= 1)
      {
        int offset = (buffer[2] * 256) + buffer[3];
        buffer[0] = 1;
        buffer[1] = buffer[4];
        for(byte i = 0; i < buffer[1]; i++)
        {
          buffer[2 + i] = EEPROM.read(offset + i);
        }
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x70: // defineWire
      if(buffer[1] == 0)
      {
        _wireDefined = true;
        Wire.begin();
        buffer[0] = 1;
        buffer[1] = 0;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x71: // setWire
      if(buffer[1] >= 2 &&
      buffer[2] <= 127 &&
      _wireDefined)
      {
        Wire.beginTransmission(buffer[2]);
        for(byte i = 0; i < (buffer[1] - 1); i++)
        {
          Wire.write(buffer[3 + i]);
        }
        buffer[0] = 1;
        buffer[1] = 1;
        buffer[2] = Wire.endTransmission();
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    case 0x72: // getWire
      if(buffer[1] == 2 &&
      buffer[2] <= 127 &&
      buffer[3] >= 1 &&
      _wireDefined)
      {
        Wire.requestFrom(buffer[2], buffer[3]);
        byte requestedLength = buffer[3];
        byte length;
        buffer[0] = 1;
        for(length = 0; length < requestedLength && Wire.available(); length++)
        {
          buffer[2 + length] = Wire.read();
        }
        buffer[1] = length;
      }
      else
      {
        buffer[0] = 0;
        buffer[1] = 0;
      }
      break;
    default:
      buffer[0] = 0;
      buffer[1] = 0;
      break;
  }
}

void Teleduino328Class::loadPresets()
{
  if(EEPROM.read(0) == '#' &&
  EEPROM.read(1) == 0 &&
  EEPROM.read(2) == '#')
  {
    // Pin presets
    if(EEPROM.read(2) == '#' &&
    EEPROM.read(25) == '#' &&
    EEPROM.read(48) == '#')
    {
      for(byte i = 0; i < 22; i++)
      {
        if(i < TD328_PINS)
        {
          switch(EEPROM.read(3 + i))
          {
            case 0:
              definePinMode(i, INPUT);
              break;
            case 1:
              definePinMode(i, OUTPUT);
              switch(EEPROM.read(26 + i))
              {
                case 0:
                  setDigitalOutput(i, LOW);
                  break;
                case 1:
                  setDigitalOutput(i, HIGH);
                  break;
              }
              break;
            case 2:
              if(checkPwmPin(i))
              {
                analogWrite(i, EEPROM.read(26 + i));
              }
              break;
          }
        }
      }
    }
    // Shift register presets
    if(EEPROM.read(48) == '#' &&
    EEPROM.read(57) == '#' &&
    EEPROM.read(122) == '#')
    {
      if(TD328_SHIFT_REGISTERS >= 1)
      {
        if(checkDigitalPin(EEPROM.read(49)) &&
        checkDigitalPin(EEPROM.read(50)) &&
        checkDigitalPin(EEPROM.read(51)))
        {
          _shiftRegisters[0].clockPin = EEPROM.read(49);
          definePinMode(_shiftRegisters[0].clockPin, OUTPUT);
          _shiftRegisters[0].dataPin = EEPROM.read(50);
          definePinMode(_shiftRegisters[0].dataPin, OUTPUT);
          _shiftRegisters[0].latchPin = EEPROM.read(51);
          definePinMode(_shiftRegisters[0].latchPin, OUTPUT);
          if(checkDigitalPin(EEPROM.read(52)))
          {
            _shiftRegisters[0].enablePin = EEPROM.read(52);
            definePinMode(_shiftRegisters[0].enablePin, OUTPUT);
          }
          for(byte i = 0; i < 32; i++)
          {
            _shiftRegisters[0].outputs[i] = EEPROM.read(58 + i);
          }
        }
      }
      if(TD328_SHIFT_REGISTERS >= 2)
      {
        if(checkDigitalPin(EEPROM.read(53)) &&
        checkDigitalPin(EEPROM.read(54)) &&
        checkDigitalPin(EEPROM.read(55)))
        {
          _shiftRegisters[1].clockPin = EEPROM.read(53);
          definePinMode(_shiftRegisters[1].clockPin, OUTPUT);
          _shiftRegisters[1].dataPin = EEPROM.read(54);
          definePinMode(_shiftRegisters[1].dataPin, OUTPUT);
          _shiftRegisters[1].latchPin = EEPROM.read(55);
          definePinMode(_shiftRegisters[1].latchPin, OUTPUT);
          if(checkDigitalPin(EEPROM.read(56)))
          {
            _shiftRegisters[1].enablePin = EEPROM.read(56);
            definePinMode(_shiftRegisters[1].enablePin, OUTPUT);
          }
          for(byte i = 0; i < 32; i++)
          {
            _shiftRegisters[1].outputs[i] = EEPROM.read(90 + i);
          }
        }
      }
    }
    // Serial presets
    if(EEPROM.read(122) == '#' &&
    EEPROM.read(127) == '#')
    {
      switch(EEPROM.read(123))
      {
        case 0:
          Serial.begin(300);
          break;
        case 1:
          Serial.begin(1200);
          break;
        case 2:
          Serial.begin(2400);
          break;
        case 3:
          Serial.begin(4800);
          break;
        case 4:
          Serial.begin(9600);
          break;
        case 5:
          Serial.begin(14400);
          break;
        case 6:
          Serial.begin(19200);
          break;
        case 7:
          Serial.begin(28800);
          break;
        case 8:
          Serial.begin(38400);
          break;
        case 9:
          Serial.begin(57600);
          break;
        case 10:
          Serial.begin(115200);
          break;
      }
      Serial.flush();
    }
    // Servo presets
    if(EEPROM.read(127) == '#' &&
    EEPROM.read(134) == '#' &&
    EEPROM.read(141) == '#')
    {
      for(byte i = 0; i < 6; i++)
      {
        if(i < TD328_SERVO_INSTANCES)
        {
          if(checkDigitalPin(EEPROM.read(128 + i)))
          {
            _servoInstances[i].servo.attach(EEPROM.read(128 + i));
            if(EEPROM.read(135 + i) <= 180)
            {
              _servoInstances[i].servo.write(EEPROM.read(135 + i));
            }
          }
        }
      }
    }
    // Wire preset
    if(EEPROM.read(141) == '#' &&
    EEPROM.read(143) == '#')
    {
      if(EEPROM.read(142) == 1)
      {
        _wireDefined = true;
        Wire.begin();
      }
    }
  }
}

void Teleduino328Class::pinTimers()
{
  for(byte i = 0; i < TD328_PINS; i++)
  {
    if(_pins[i].expireAction < 255 && _pins[i].expireTime <= millis() && (millis() - _pins[i].expireTime) <= 3600000 && (_pins[i].expireTime - millis()) >= 3600000)
    {
      setDigitalOutput(i, _pins[i].expireAction);
      _pins[i].expireAction = 255;
      _pins[i].expireTime = 0;
    }
  }
}

void Teleduino328Class::reset()
{
  delay(100);
  for(byte i = 0; i < TD328_PINS; i++)
  {
    setDigitalOutput(i, LOW);
  }
  void(* teleduino328ArduinoReset) (void) = 0;
  teleduino328ArduinoReset();
}

boolean Teleduino328Class::setDigitalOutput(byte pin, byte value, unsigned int time, byte save)
{
  if(checkDigitalPin(pin) && _pins[pin].mode == OUTPUT)
  {
    if(save)
    {
      if(time == 0 &&
      EEPROM.read(0) == '#' &&
      EEPROM.read(1) == 0 &&
      EEPROM.read(2) == '#')
      {
        if(EEPROM.read(25) != '#')
        {
          EEPROM.write(25, '#');
        }
        if(EEPROM.read(48) != '#')
        {
          EEPROM.write(48, '#');
        }
        if(EEPROM.read(3 + pin) != 1)
        {
          EEPROM.write(3 + pin, 1);
        }
        if(EEPROM.read(26 + pin) != value)
        {
          EEPROM.write(26 + pin, value);
        }
      }
      else
      {
        return false;
      }
    }
    digitalWrite(pin, value);
    _pins[pin].expireAction = 255;
    _pins[pin].expireTime = 0;
    if(time > 0)
    {
      _pins[pin].expireAction = !value;
      _pins[pin].expireTime = millis() + time;
      if(_pins[pin].expireTime < 1)
      {
        _pins[pin].expireTime = 1;
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}

boolean Teleduino328Class::setDigitalOutput(byte pin, byte value, unsigned int time)
{
  return setDigitalOutput(pin, value, time, 0);
}

boolean Teleduino328Class::setDigitalOutput(byte pin, byte value)
{
  return setDigitalOutput(pin, value, 0, 0);
}

void Teleduino328Class::setStatusLed(unsigned int count, boolean forever, long timeout)
{
  while(1)
  {
    for(unsigned int i = 0; i < count; i++)
    {
      if(_statusLedPin > -1)
      {
        setDigitalOutput(_statusLedPin, HIGH);
        delay(100);
        setDigitalOutput(_statusLedPin, LOW);
        delay(200);
      }
      else
      {
        delay(300);
      }
    }
    delay(500);
    if(!forever)
    {
      if(timeout <= 0)
      {
        break;
      }
      else
      {
        timeout = timeout - (count * 300) - 500;
      }
    }
  }
}

void Teleduino328Class::setStatusLed(unsigned int count, boolean forever)
{
  setStatusLed(count, forever, 0);
}

void Teleduino328Class::setStatusLed(unsigned int count, long timeout)
{
  setStatusLed(count, false, timeout);
}

void Teleduino328Class::setStatusLed(unsigned int count)
{
  setStatusLed(count, false, 0);
}

boolean Teleduino328Class::setStatusLedPin(byte pin)
{
  if(checkDigitalPin(pin))
  {
    _statusLedPin = pin;
    definePinMode(_statusLedPin, OUTPUT);
    return true;
  }
  else
  {
    return false;
  }
}

void Teleduino328Class::shiftRegisters()
{
  for(byte i = 0; i < TD328_SHIFT_REGISTERS; i++)
  {
    if(checkDigitalPin(_shiftRegisters[i].clockPin) &&
    checkDigitalPin(_shiftRegisters[i].dataPin) &&
    checkDigitalPin(_shiftRegisters[i].latchPin))
    {
      for(char j = 31; j >= 0; j--)
      {
        shiftOut(_shiftRegisters[i].dataPin, _shiftRegisters[i].clockPin, MSBFIRST, _shiftRegisters[i].outputs[j]);
      }
      setDigitalOutput(_shiftRegisters[i].latchPin, HIGH);
      setDigitalOutput(_shiftRegisters[i].latchPin, LOW);
      if(checkDigitalPin(_shiftRegisters[i].enablePin))
      {
        setDigitalOutput(_shiftRegisters[i].enablePin, HIGH);
      }
    }
  }
}

void Teleduino328Class::shiftRegisterTimers()
{
  for(byte i = 0; i < TD328_SHIFT_REGISTER_TIMERS; i++)
  {
    if(_shiftRegisterTimers[i].shiftRegister < 255)
    {
      boolean blank = true;
      for(byte j = 0; j < 32; j++)
      {
        if(_shiftRegisterTimers[i].outputs[j] > 0)
        {
          blank = false;
          break;
        }
      }
      if(blank)
      {
        _shiftRegisterTimers[i].shiftRegister = 255;
        _shiftRegisterTimers[i].action = 255;
        _shiftRegisterTimers[i].expireTime = 0;
        for(byte j = 0; j < 32; j++)
        {
          _shiftRegisterTimers[i].outputs[j] = 0;
        }
      }
    }
    if(_shiftRegisterTimers[i].shiftRegister < 255 && _shiftRegisterTimers[i].expireTime <= millis() && (millis() - _shiftRegisterTimers[i].expireTime) <= 3600000 && (_shiftRegisterTimers[i].expireTime - millis()) >= 3600000)
    {
      for(byte j = 0; j < 32; j++)
      {
        for(byte k = 0; k < 8; k++)
        {
          byte currentBit = 1;
          currentBit = currentBit << k;
          if((_shiftRegisterTimers[i].outputs[j] & currentBit) > 0)
          {
            switch(_shiftRegisterTimers[i].action)
            {
              case 0:
                if((_shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] & currentBit) > 0)
                {
                  _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] = _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] - currentBit;
                }
                break;
              case 1:
                if((_shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] & currentBit) == 0)
                {
                  _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] = _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] + currentBit;
                }
                break;
              case 2:
                if((_shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] & currentBit) > 0)
                {
                  _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] = _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] - currentBit;
                }
                else if((_shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] & currentBit) == 0)
                {
                  _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] = _shiftRegisters[_shiftRegisterTimers[i].shiftRegister].outputs[j] + currentBit;
                }
                break;
            }
          }
        }
      }
      _shiftRegisterTimers[i].shiftRegister = 255;
      _shiftRegisterTimers[i].action = 255;
      _shiftRegisterTimers[i].expireTime = 0;
      for(byte j = 0; j < 32; j++)
      {
        _shiftRegisterTimers[i].outputs[j] = 0;
      }
    }
  }
}

Teleduino328Class Teleduino328;
