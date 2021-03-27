#include "Arduino.h"
#include <cstdio> // printf
#include <iostream> // cout

// You can get the result e.g with  screen /dev/cu.usbmodemFA141 115200
void setup(){
    // Serial is mapped to the Pico USB output. 
    Serial.begin();
}

void loop(){
    Serial.println("Hallo world! from Arduino Serial");
    std::cout << "Hello World! from C++ runtime" <<  std::endl;
    printf("Hello World! from C runtime\n");

    delay(1000);
}