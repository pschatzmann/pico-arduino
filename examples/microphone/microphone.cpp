#include "Arduino.h"
#include "AudioTools/AudioTools.h"

// define global data
Microphone mic(1024,4);
int16_t *dataArray;
int actualDataLength=0; 
long counter;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin();
    delay(10000);
    // we actiave the log with log level info
    AudioLogger::instance().begin(Serial); 

    // sampling at indicated rate (e.g. 22050) at GP26 running on core 1
    Serial.println("Starting microphone...");
    mic.begin(22050, GP26, 1);
    Serial.println("Microphone started");
}

// print sampling rate every 10 seconds
void printSamplingRates() {
    if (counter%100000 == 0){
        Serial.print("Sampling rates (to be/effective): ");
        Serial.print(mic.sampleRate());
        Serial.print(" ");
        Serial.println();
    }
}

// print sampling rate every 10 seconds
void printSamples() {
    for (int j=0;j<actualDataLength;j++){
        Serial.println(dataArray[j]);
    }
}

// the loop function runs over and over again forever
void loop() {
    actualDataLength = mic.data(dataArray);
    printSamples();
//  printSamplingRates();
}