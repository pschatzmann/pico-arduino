#pragma once

/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef HOST
#define HOST
#endif

#include <stdint.h>
#include "pins_arduino.h"
#include "ArduinoAPI.h"
#include "PicoLogger.h"
#include "PicoHardwareSerial.h"
#include "PicoHardwareSPI.h"
#include "PicoGPIOFunction.h"
//#include "PicoHardwareI2C.h"
#include "PicoTone.h"
#include "PicoTimer.h"
#include "PicoThread.h"
#include "PicoSemaphore.h"
#include "PicoMulticoreFifo.h"
#include "PicoQueue.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

// global variables
extern PicoDefaultSerial Serial; // pico_stdio
extern PicoHardwareSerial Serial1;
extern PicoHardwareSerial Serial2; 
extern PicoLogger Logger;   // Support for logging
extern PicoGPIOFunction GPIOFunction;
// Pico specific global methods
// Provides the on board temperature in Celsius
int getTemperature();
