#include "Arduino.h"

Thread thread;

void methodOn2ndProcessor(){
    while(true){
        Serial.println("Thread 1");
        delay(1000);
    }
}
 
// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin();
    Logger.begin(Serial, PicoLogger::Info); // we actiave the log with log level info
    Serial.println("Starting...");
    thread.start(methodOn2ndProcessor);
    delay(500);
}

// the loop function runs over and over again forever
void loop() {
    Serial.println("Thread 0");
    delay(1000);
}