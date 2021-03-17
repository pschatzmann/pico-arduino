#include "Arduino.h"

// Queue( int maxCount = 100,bool blocking=true, bool withLock=true ){
Queue<long> queue(100, true, true);
Thread thread;

void methodOn2ndProcessor(void*ptr){
  long data;
  // we just pop data from the queue and print it to the console
  while(true){
    queue.pop(data);
    Serial.println(data);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin();
    Logger.begin(Serial, PicoLogger::Info); // we actiave the log with log level info
    Serial.println("Starting...");
    
    thread.start(methodOn2ndProcessor); // we start the 2nd processor
}

// the loop function runs over and over again forever
void loop() {
  for (long j=0; j<0xFFFFFFFF; j++){
    queue.push(j); // we just push data to the queu
  }
}