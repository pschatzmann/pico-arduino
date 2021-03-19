#include "Arduino.h"

void setup(){
    Serial.begin();
    // waiting for the user to connect
    while(!Serial){
        delay(500);
    }
    Serial.println("C, F");
}

void loop(){
    Serial.print(temperature());
    Serial.print(", ");
    Serial.println(temperatureF());
    delay(10000);
}
