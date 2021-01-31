#include "Arduino.h"

bool active;
TimerAlarmRepeating timer;

bool blink(repeating_timer_t *rt) {
    active = !active; // toggle state
    digitalWrite(LED_BUILTIN, active);  
    return true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // execute blink every 1000 ms (=1sec)
  timer.start(blink, 1000);
}

void loop() {
}