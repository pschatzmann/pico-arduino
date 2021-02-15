/**
 * @brief Just a little test program which generates sound data and prints it to the console
 * @copyright Copyright (c) 2021 Phil Schatzmann
 * 
 */

#include "Arduino.h"
#include "AudioTools/AudioTools.h"

// define global data
NoiseGenerator<int16_t> input(1000.0);
//SineWaveGenerator<int16_t> input(1000.0);
int16_t *dataArray;
int actualDataLength=0; 
long counter;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin();

    Serial.println("Starting...");
    input.begin(); // for noise
//    input.begin(44100, 2048);  // for sine wave
    Serial.println("Started");
}


// print sampling rate every 10 seconds
void printSamples() {
    for (int j=0;j<actualDataLength;j++){
        Serial.println(dataArray[j]);
    }
}

// the loop function runs over and over again forever
void loop() {
    actualDataLength = input.data(dataArray);
    printSamples();
}