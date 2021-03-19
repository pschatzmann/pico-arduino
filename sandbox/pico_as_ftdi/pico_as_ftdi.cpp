#include "Arduino.h"
#include "SoftwareSerial.h"

enum Mode {Simple, ESP01_DATA, ESP01_FASH};
SoftwareSerial SerialS;
const int rx_pin = GP9;
const int tx_pin = GP8;
const int baud = 115200;
Mode mode = ESP01_DATA;
// ESP01 pins
//const int rst_pin = GP20;
const int en_pin = GP18;
const int gpio0_pin = GP18;

void setupMode() {
    switch(mode){
        case ESP01_DATA:
            pinMode(en_pin, OUTPUT);    
            //pinMode(rst_pin, OUTPUT);    
            pinMode(gpio0_pin, OUTPUT);  
            digitalWrite(en_pin, HIGH); 
            //digitalWrite(rst_pin, HIGH);    
            digitalWrite(gpio0_pin, LOW);    
            break;
        case ESP01_FASH: 
            pinMode(en_pin, OUTPUT);    
            //pinMode(rst_pin, OUTPUT);    
            pinMode(gpio0_pin, OUTPUT);  
            digitalWrite(en_pin, HIGH); 
            //digitalWrite(rst_pin, HIGH);    
            digitalWrite(gpio0_pin, HIGH);    
            break;

        break;

    }
}

void setup() {
    // USB Speed is irrelevant
    Serial.begin();
    Logger.begin(Serial, PicoLogger::Info);
    while(!Serial){
        delay(100);
    }
    Serial.println("Connected...");
    // Seril speed is critical !
    Serial2.begin(baud, rx_pin, tx_pin);
}

void copy() {
    static char buffer[512];
    // Serial1 -> Serial
    if (Serial2.available()>0){
        int len = Serial2.readBytes(buffer,512);
        Serial.write(buffer,len);
        Serial.flush();
    }

    // Serial -> Serial1
    if (Serial.available()>0){
        int len = Serial.readBytes(buffer,512);
        Serial2.write(buffer,len);
        Serial2.flush();
    }
}

void loop() {
    copy();
}