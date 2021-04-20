#include "Arduino.h"
#include "pico/multicore.h"

volatile uint8_t flag = 0;

void detectUpdateHandler(){
    uint8_t confirmed_flag = 0;
    while(true){    
      if ( flag!= confirmed_flag){
        Serial.print("new value is ");
        Serial.println( flag);
        confirmed_flag =  flag;
      }
    }
}

void setup() {
    Serial.begin();
    while(!Serial);
    multicore_launch_core1(detectUpdateHandler);
    Serial.println("started...");
}

void loop() {
    flag++;
    delay(1000);
}