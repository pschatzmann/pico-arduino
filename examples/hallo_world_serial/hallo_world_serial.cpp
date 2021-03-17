#include "Arduino.h"

// we use the default baud rate which is 115200. 
// USB: You can get the result e.g with  screen /dev/cu.usbmodemFA141 115200
// UART: By default we use the following pins: UART0 tx/rx = gp0/gp1; UART1 tx/rx = gp4/gp5; 
void setup(){
    // Serial is mapped to the default Pico output - which usess USB
    Serial.begin();
    // waiting to connect to USB
    while(!Serial){
        delay(100);
    }
    Logger.begin(Serial, PicoLogger::Info);
    // Serial output to TX/RX pins
    int rx = GP13;
    int tx = GP12;
    Serial1.begin(115200, SERIAL_8N1, rx, tx);
    Serial2.begin(115200);
}

void loop(){
    Serial.println("Hallo world - USB");
    Serial1.println("Hallo world - Serial1");
    Serial2.println("Hallo world - Serial2");
    delay(1000);
}