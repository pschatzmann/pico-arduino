#include "Arduino.h"

const int sampling_rate = -2000; //20000;
const pin_size_t mic_pin = 27;
TimerAlarmRepeating timer;
int min_value=20000;
int max_value=0;

// read a sample from the microphone into the buffer
bool microphoneRead(repeating_timer_t *rt) {
    // add a sample to the buffer
    int value = analogRead(mic_pin);
    min_value = min(min_value,value);
    max_value = max(max_value, value);
    Serial.printf("%d, min: %d, max: %d, avg: %f\n", value, min_value, max_value, 0.5 * (min_value + max_value));
    return true;
}

// Setup Sound processing
void setup(){
    Serial.begin();
    Logger.begin(Serial,PicoLogger::Warning);
    while(!Serial);

    Serial.println("setup...");

    // start sampling of sound from microphone with 44100 Hz
    uint64_t time = (1000000 / sampling_rate);
    timer.start(microphoneRead, time, US);

}


// Just print out the calculated total sampling rate every 10 seconds
void loop() {
}