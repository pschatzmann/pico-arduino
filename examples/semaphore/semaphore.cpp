#include "Arduino.h"

//Semaphore(int maxPermits=1, int initalPermits=1)
Semaphore semaphore;
Thread thread;
long counter;

void increment() {
    // when 2 processes are working on one resource we need to coordinate the access e.g. with a semaphore
    semaphore.aquire();
    Serial.println(counter++);
    semaphore.release();
}

void methodOn2ndProcessor(void*ptr){
    while(true){
        increment();
    }
}
 
// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9800);
    Logger.begin(Serial, PicoLogger::Info); // we actiave the log with log level info

    Serial.println("Starting...");
    thread.start(methodOn2ndProcessor);
}

// the loop function runs over and over again forever
void loop() {
    increment();
}