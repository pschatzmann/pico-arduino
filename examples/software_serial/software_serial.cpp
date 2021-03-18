
#include "Arduino.h"
#include "SoftwareSerial.h"

SoftwareSerial SofwareSerial;
int tx_pin = GP21;
int rx_pin = GP22;
int baud = 115200;

void setup(){
    // give us some time to connect via the termial emulation
    delay(10000);

    // start
    SofwareSerial.begin(baud, tx_pin, rx_pin);
    SofwareSerial.println("press any key to start...");
    while(SofwareSerial.read()==-1) {
        delay(100);
    }
}

void loop(){
    SofwareSerial.println("test");
    delay(1000);
}