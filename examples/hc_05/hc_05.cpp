#include "Arduino.h"
#include "SoftwareSerial.h"

SoftwareSerial SerialS;
int rx = GP15;
int tx = GP14;


// Arduino setup executed once 
void setup() {
    // set up Serial on USB for logging and wait for the user to connect
    Serial.begin(115200);
    while(!Serial){
        delay(100);
    }
    Serial.println("setup...");

    // use the default communication baud rate for the HC05
    SerialS.begin(9600, tx, rx);
    Serial.print("We are in Data Mode: ready for processing data...");

}

// we just copy the data between Serial and SerialS - so if you enter a string in the Bluetooth Termial
// you should see it in the USB Terminal 
void copyData() {
    static uint8_t buffer[180];
    // SerialS -> Serial
    if (SerialS.available()>0){
        int len = SerialS.readBytes(buffer,180);
        Serial.write(buffer,len);
        Serial.flush();
    }

    // Serial -> SerialS
    if (Serial.available()>0){
        int len = Serial.readBytes(buffer,180);
        SerialS.write(buffer,len);
        SerialS.flush();
    }
}

// repeated processing: We just send some data to the HC05 device
long ping_timeout;
void loop(){
    if (millis()>ping_timeout) {
        SerialS.println("ping...");
        ping_timeout = millis() + (1000 * 10);
    }
    copyData();
}