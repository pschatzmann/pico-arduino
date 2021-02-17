#include "Arduino.h"
#include "USB.h"

USBAudio audio;
SineWaveGenerator<int16_t> toneGenerator(32000);
int sample_rate=AUDIO_SAMPLE_RATE;
int testFrequency=220;

void setup() {
    //Serial1.begin(115200);
    toneGenerator.begin(sample_rate, testFrequency);
    //Serial1.println("Starting audio...");
    audio.begin(toneGenerator, sample_rate);
}

void loop(){

}