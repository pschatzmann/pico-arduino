    
#include "Arduino.h"

// on the Pico analog input is available on GP26-GP28    
const pin_size_t mic_pin = 27;

void setup() {
    Serial.begin();
}

void loop(){
    Serial.println(analogRead(mic_pin));
}