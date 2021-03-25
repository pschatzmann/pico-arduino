#include "Arduino.h"
#include "PicoDMA.h"

const int buffer_len = 10000;
uint16_t src[buffer_len];
uint16_t dst[buffer_len];
uint16_t initial_value = 1;
PicoDMA<uint16_t> dma;
int channel;


// we check if the array constains the expected data
void checkData(uint16_t *data, uint16_t expected) {
    // check data
    for (int j=0;j<buffer_len;j++){
        if (data[j]!=expected){
            Serial.print("Error at index ");
            Serial.print(j);
            Serial.print(": ");
            Serial.print(expected);
            Serial.print(" - ");
            Serial.println(data[j]);
        }
    }
}

void setup(){
    // Serial is mapped to USB output - we wait for the user to connect
    Serial.begin();
    while(!Serial);

    // get a new DMA channel
    channel = dma.getChannel();
    // initialize source buffer with initial_value
    dma.set(channel, src, initial_value, buffer_len);
    Serial.println("processing...");
    dma.wait(channel);
    checkData(src, initial_value);
    Serial.println("Data Initialized");

    // start the copy 
    dma.copy(channel, dst, src, buffer_len);
    Serial.println("processing...");
    dma.wait(channel);
    checkData(dst, initial_value);
    Serial.println("Data copied");
}

void loop(){
    // just print out a * character from loop
    Serial.print("*");
    if (!dma.isBusy(channel)){
        // just print out + character to show that copy is done and we restart copy
        Serial.print("+");
        dma.copy(channel, dst, src, buffer_len);    
    }
}