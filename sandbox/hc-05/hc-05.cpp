#pragma once

#include "Arduino.h"
#include "SoftwareSerial.h"

SofwareSerial SerialS;
HC05 hc05;
int rx = GP05;
int tx = GP06;
int key = GP07;

// Change HC05 settings if in AT mode
void setupHC05() {
    // set up hc05
    SerialS.begin(38400);
    hc05.begin(SerialS, rx, tx, key, Serial);

    // if we started in AT mode we change some settings
    hc05.setAtMode();
    if (hc05.isAtMode()){
        Serial.print("We are in AT Mode: setting up the device...");
        Serial.print("Version: ");
        Serial.print(hc05.version());
        hc05.restoreDefaultStatus();
        hc05.setName("MyPico");
        hc05.setPasskey("4321");
        hc05.setBaudRate(R115200);

        Serial.println("Setup completed - Please reboot now");
        while(true){
            delay(1000);
        }
    } 
}

// Arduino setup executed once 
void setup() {
    // set up Serial on USB for logiing
    Serial.begin(115200);
    // press the key on the HC05 to set it into AT mode
    setupHC05();

    // use the changed communication baud rate for hc05
    SerialS.begin(115200, rx, tx);
    Serial.print("We are in Data Mode: ready for processing data...")

}

// repeated processing: We just send some data to the HC05 device
void loop(){
    delay(1000);
    SerialS.println("ping...")
}