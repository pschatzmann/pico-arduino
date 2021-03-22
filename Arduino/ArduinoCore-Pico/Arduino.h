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
#include <stdlib.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include "pins_arduino.h"
#include "ArduinoAPI.h"
#include "PicoLogger.h"
#include "PicoHardwareSerial.h"
#include "PicoHardwareSPI.h"
#include "PicoPinFunction.h"
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

// Global variables
extern PicoSerialUSB Serial; // pico_stdio
extern PicoSerialUART Serial1;
extern PicoSerialUART Serial2; 
extern PicoLogger Logger;   // Support for logging
extern PicoPinFunction PinFunction;

// Pico specific global methods
// Provides the on board temperature in Celsius
int temperature();
// Temperature in Fahrenheit for our friends in Belize, Paulau, the Cayman Islands and the Bahamas
int temperatureF();

// Some Arduino libraries rely on the following
char *dtostrf (double __val, signed char __width, unsigned char __prec, char *__s);
char *dtostre (double __val, char *__s, unsigned char __prec, unsigned char __flags);
