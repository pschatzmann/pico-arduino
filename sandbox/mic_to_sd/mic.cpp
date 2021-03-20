    
#include "Arduino.h"

// on the Pico analog input is available on GP26-GP28    
const pin_size_t mic_pin = 27;

void setup() {
    Serial.begin();
    // we wait for the user to connect
    while(!Serial){
        delay(10);
    }
    Serial.println("setup...");
    Logger.begin(Serial, PicoLogger::Info);
}

void loop(){
    Serial.print("measurement: ");
    int value = analogRead(mic_pin);
    Serial.println(value);
}