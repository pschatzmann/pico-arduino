#include "Arduino.h"

uint8_t buffer[180];
const uint32_t BAUD = 115200;

void setup() {
    Serial.begin();
    Serial2.begin(BAUD);
}

void loop() {
    // Serial1 -> Serial
    if (Serial1.available()>0){
        int len = Serial1.readBytes(buffer,180);
        Serial.write(buffer,len);
        Serial.flush();
    }

    // Serial -> Serial1
    if (Serial.available()>0){
        int len = Serial.readBytes(buffer,180);
        Serial1.write(buffer,len);
        Serial1.flush();
    }

}