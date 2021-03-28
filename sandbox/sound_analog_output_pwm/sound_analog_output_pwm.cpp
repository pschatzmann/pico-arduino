#include "Arduino.h"
#include "PicoPWM.h"
#include "sample.h"

const int sound_pin = GP15;
const int frequencyHz = 30000; // we use a frequency which is not audable
const int sampling_rate = -5000; // sampling rate of our recorded data
const int maxValue = 255;
int current_sample = 0;

PicoPWM pwm(frequencyHz, maxValue);
TimerAlarmRepeating timer;

bool playSample(repeating_timer_t *rt) {
    // write sample as pwm output 
    pwm.write(sound_pin, sample1_raw[current_sample++]);
    // prevent an overflow
    if (current_sample>=sample1_raw_len){
        current_sample = 0;
        Serial.println("restarting to play...");
    }
    return true;
}

void setup() {
    Serial.begin();
    while(!Serial);
    Logger.begin(Serial,PicoLogger::Debug);

    // start pwm output
    pwm.begin(sound_pin);
    
    Serial.println("starting to play...");

    // start sampling of sound from microphone 
    uint64_t time = (1000000 / sampling_rate) ;
    timer.start(playSample, time, US);
}

void loop() {
}