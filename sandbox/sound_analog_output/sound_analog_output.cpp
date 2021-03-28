#include "Arduino.h"
#include "sample.h"
#include "Tools/SerialDAC.h"

const int sound_pin = GP0;
const int sampling_rate = 5000; 
const SerialDAC dac(Serial1);
int current_sample = 0;
int16_t *data = (int16_t *)sample1_raw;
int len = sample1_raw_len / 2;

uint16_t ntohs(uint16_t num){
    return (num>>8) | (num<<8);
}

void setup() {
    SerialUSB.begin();
    while(!SerialUSB);
    uint baudRate = dac.baudRate(sampling_rate);
    SerialUSB.printf("Baud rate must be set to %u\n", baudRate);
    Serial1.begin(baudRate, -1, sound_pin);
    SerialUSB.println("Starting to play");
}

void loop() {
    // write sample as pwm output
    uint16_t value = ntohs(data[current_sample++]); 
    dac.write(value);
    //Serial.println(value);
    // prevent an overflow
    if (current_sample>=len){
        current_sample = 0;
        SerialUSB.println("Restarting to play");
    }

}