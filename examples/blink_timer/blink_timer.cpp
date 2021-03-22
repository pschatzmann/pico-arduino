#include "Arduino.h"

bool active;
TimerAlarmRepeating timer;
const int led_pin = GP15; //LED_BUILTIN;

bool blink(repeating_timer_t *rt) {
    active = !active; // toggle state
    digitalWrite(led_pin, active);  
    return true;
}

void setup() {
  pinMode(led_pin, OUTPUT);
  // execute blink every 1000 ms (=1sec)
  timer.start(blink, 1000);
}

void loop() {
}