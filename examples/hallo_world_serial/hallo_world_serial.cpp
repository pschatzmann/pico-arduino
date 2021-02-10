#include "Arduino.h"

// we use the default baud rate which is 115200. 
// You can get the result e.g with  screen /dev/cu.usbmodemFA141 115200
void setup(){
    // Serial is mapped to the default Pico output. 
    Serial.begin();
    Logger.begin(Serial, PicoLogger::Info);
    // Serial1 is mapped to the uart0 using gpio0 and gpio1. 
    Serial1.begin(115200);
}

void loop(){
    Serial1.println("Hallo world");
    delay(1000);
}