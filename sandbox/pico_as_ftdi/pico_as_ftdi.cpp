#include "Arduino.h"


#ifdef USE_SOFTSERIAL
#include "SoftwareSerial.h"
SoftwareSerial SerialS;
#endif

enum Mode {Simple, ESP01_DATA, ESP01_FASH};
const int rx_pin = GP13;
const int tx_pin = GP12;
const int baud = 115200;
Mode mode = ESP01_DATA;
// ESP01 pins
//const int rst_pin = GP20;
const int en_pin = GP18;
const int gpio0_pin = GP18;
Stream *altDest;

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
    Serial.setTimeout(100);
    // Seril speed is critical !
#ifdef USE_SOFTSERIAL
    SerialS.begin(baud, rx_pin, tx_pin);
    SerialS.setTimeout(100);
    altDest = &SerialS;

#else
    Serial1.begin(baud, rx_pin, tx_pin);
    Serial1.setTimeout(100);
    altDest = &Serial1;
#endif
}

void copy() {
    static char buffer[512];
    // Serial1 -> Serial
    if (altDest->available()>0){
        int len = altDest->readBytes(buffer,512);
        Serial.write(buffer,len);
        Serial.flush();
    }

    // Serial -> Serial1
    if (Serial.available()>0){
        int len = Serial.readBytes(buffer,512);
        altDest->write(buffer,len);
        altDest->flush();
    }
}

void loop() {
    copy();
}