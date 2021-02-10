/**
 * We just send a sound out to the I2S interface 
 */

#include "Arduino.h"
#include "I2S/I2S.h"
#include "Audiotools/Audiotools.h"
#include "Audiotools/Notes.h"

I2S i2s;
SineWaveGenerator<int16_t> sound;
int samplingRate = 44100;
int16_t buffer[1024];
int bufferSize = sizeof(buffer);
uint start_pin = 1; // sd = 1, sck = 2, ws = 3,

void setup() {
  Serial.begin();
  Serial.println("Starting to generate sound...");

  sound.begin(samplingRate, E3);
  i2s.begin(start_pin, samplingRate);
}


void loop() {
  while (true){
    int sizeReceived = sound.readBytes((uint8_t *)buffer, bufferSize);
    i2s.write((uint8_t *)buffer, sizeReceived); 
  }
}

