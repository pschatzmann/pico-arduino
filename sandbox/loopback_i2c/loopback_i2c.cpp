/**
 * We just send a out data via I2S and receive them  on the second cpu core
 * pin connectsion: 1 <-> 4; 2 <-> 5; 3 <-> 6 
 */

#include "Arduino.h"
#include "I2C.h"
#include "Audiotools/Audiotools.h"
#include "Audiotools/Notes.h"

const int bufferSize = 44100;
int16_t buffer[bufferSize];
I2C tx;
I2C rx;
int slave_id = 1;
Thread thead;


void receive() {
  int16_t rx_buffer[bufferSize];

  while(true){
    int sizeReceived = rx.readBytes((uint8_t *)buffer, bufferSize*2); 
    // just print out all value
    for (j=0;j<sizeReceived;j++){
      Serial.println(rx_buffer[j]);
    }
  }
}

void setup() {
  Serial.begin();
  Serial.println("Starting to process data...");
  thread.start(receive);

  tx.begin(); // master
  rx.begin(slave_id); // slave on address 1
}

void loop() {
  while (true){
    // fill buffer with some values
    for (j=0;j<bufferSize;j++){
      buffer[j]=j;
    }
    // write data to I2S address 1
    tx.beginTransaction(slave_id);
    tx.write((uint8_t *)buffer, bufferSize*2); 
    tx.endTransaction();
  }
}

