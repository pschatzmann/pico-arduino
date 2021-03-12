/**
 * We just send a out data via I2S and receive them  on the second cpu core
 * pin connectsion: 1 <-> 4; 2 <-> 5; 3 <-> 6 
 */

#include "Arduino.h"
#include "I2S/I2S.h"
#include "Audiotools/Audiotools.h"
#include "Audiotools/Notes.h"

const int bufferSize = 44100;
int samplingRate = 44100;
int16_t buffer[bufferSize];
uint start_tx_pin = 2; // sd = 2, sck = 3, ws = 4,
uint start_rx_pin = 6; // sd = 6, sck = 7, ws = 8,
I2S i2s_tx(pio0);
I2S i2s_rx(pio1);
Thread thread;


void receive() {
  int16_t rx_buffer[bufferSize];
  while(true){
    int sizeReceived = i2s_rx.readBytes((uint8_t *)buffer, bufferSize*2); 
    // just print out all value
    for (int j=0;j<sizeReceived;j++){
      Serial.println(rx_buffer[j]);
    }
  }
}

void setup() {
  Serial.begin();
  delay(10000);
  Serial.println("Starting to process data...");
  thread.start(receive);
  // start 2 i2s interfaces: a master and a slave
  i2s_tx.begin(start_tx_pin, samplingRate, true);
  i2s_rx.begin(start_rx_pin, samplingRate, false);

}

void loop() {
  while (true){
    // fill buffer with some values
    for (int j=0;j<bufferSize;j++){
      buffer[j]=j;
    }
    // write data to I2S 
    Serial.println("Writing");
    i2s_tx.write((uint8_t *)buffer, bufferSize*2); 
  }
}

