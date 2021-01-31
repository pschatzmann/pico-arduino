#include "Arduino.h"

Thread thread;
MuliticoreFifo fifo;

void methodOn2ndProcessor(){
  while(true){
    uint32_t data;
    fifo.pop(data);
    Serial.println(data);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin();
    Logger.begin(Serial, PicoLogger::Info); // we actiave the log with log level info
    Serial.println("Starting...");

    thread.start(methodOn2ndProcessor);
}

// the loop function runs over and over again forever
void loop() {
  for (uint32_t j=0; j<0xFFFF; j++){
    fifo.push(j);
  }
}