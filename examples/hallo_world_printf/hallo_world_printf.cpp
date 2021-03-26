#include "Arduino.h"

int counter=0;
const float f = 1.234;

void setup(){
    // Serial is mapped to the USB Pico output. 
    Serial.begin();
}

void loop(){
    Serial.printf("This is a decimal number %d - a constant float %f - and a '%s' string\n", counter++, f, "test");
    delay(1000);
}