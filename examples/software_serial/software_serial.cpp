
#include "Arduino.h"
#include "SoftwareSerial/SoftwareSerial.h"

SoftwareSerial SofwareSerial;
int tx_pin = GP22;

void setup(){
    pinMode(tx_pin, OUTPUT);
    SofwareSerial.begin(9600, tx_pin);
}

void loop(){
    SofwareSerial.println("test");
    delay(1000);
}