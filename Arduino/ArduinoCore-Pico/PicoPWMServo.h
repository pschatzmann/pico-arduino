#pragma once

#include "PicoPWM.h"

// the shortest pulse sent to a servo  
#ifndef MIN_PULSE_WIDTH
#define MIN_PULSE_WIDTH       544
#endif
// the longest pulse sent to a servo 
#ifndef MAX_PULSE_WIDTH
#define MAX_PULSE_WIDTH      2400
#endif
// default pulse width when servo is attached
#ifndef DEFAULT_PULSE_WIDTH
#define DEFAULT_PULSE_WIDTH  1500
#endif
// minumim time to refresh servos in microseconds
#ifndef REFRESH_INTERVAL 
#define REFRESH_INTERVAL    20000
#endif
// minimum in degrees
#define MIN_DEGREES             0
// maximum in degrees
#define MAX_DEGREES           180l

namespace pico_arduino {

/**
 * @brief We provide an alternative Pico implementation for the Servo class which is compatible with 
 * the Arduino Servo library.
 * 
 */

class Servo {
public:
  Servo() {
  }

  ~Servo(){
    if (pwm!=nullptr) delete pwm;
  }

  /// attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
  void attach(int pin) {
    if (pwm==nullptr){
       pwm = new PicoPWMNano(REFRESH_INTERVAL * 1000);
    }
    if (pwm!=nullptr)
      pwm->begin(pin);
    this->is_attached = true;
    this->pin = pin;
  }  

  /// as above but also sets min and max values for writes. 
  void attach(int pin, int min, int max) {
    this->min = min;
    this->max = max;
    attach(pin);
  }

  /// stops the generation of signals
  void detach() {
    is_attached = false;
    if (pwm!=nullptr)
      pwm->end(pin);
  }

  /// if value is < 200 its treated as an angle, otherwise as pulse width in microseconds  
  void write(int value) {
    int pulse_width_ms = value;
    if (value<200){
        pulse_width_ms = map(value,MIN_DEGREES, MAX_DEGREES, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    }
    writeMicroseconds(pulse_width_ms);
  }    

  /// Write pulse width in microseconds 
  void writeMicroseconds(int value) {
    pwm_value = value;
    if (pwm!=nullptr) {
      // convert value to nanoseconds
      pwm->setDutyCycle(pin, value * 1000);
    }
  }

  /// returns current pulse width as an angle between 0 and 180 degrees
  int read() {
      return map(pwm_value, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, MIN_DEGREES,MAX_DEGREES);
  }  

  /// returns current pulse width in microseconds for this servo (was read_us() in first release) 
  int readMicroseconds() {
      return pwm_value;
  }    

  /// return true if this servo is attached, otherwise false 
  bool attached() {
      return is_attached;
  }

private:
   PicoPWMNano *pwm;
   int8_t min = MIN_DEGREES;                         
   int8_t max = MAX_DEGREES;                       
   bool is_attached = false; 
   int pwm_value;
   int pin;

};

}
