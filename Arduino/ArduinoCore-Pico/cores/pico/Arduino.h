#pragma once

/**
 * Arduino.h - Main include file for the PICO Arduino SDK
 * @author Phil Schatzmann
 * @copyright GPLv3
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
