#include "Arduino.h"

// we use the default baud rate which is 115200. 
// You can get the result e.g with  screen /dev/cu.usbmodemFA141 115200
void setup(){
    // Serial is mapped to the default Pico output. 
    Serial.begin();
}

void loop(){
    Serial.print("test-");
    Serial.println("Hallo world");
    delay(1000);
}