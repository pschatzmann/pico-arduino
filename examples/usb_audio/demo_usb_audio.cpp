#include "Arduino.h"
#include "AudioTools/AudioTools.h"
#include "USB.h"

USBAudio audio = USBAudio::instance();
SineWaveGenerator<int16_t> toneGenerator(32000);
int sample_rate=44100;
int testFrequency=220;

void setup() {
    Serial.begin(115200);
    toneGenerator.begin(sample_rate, testFrequency);
    Serial.println("Starting audio...");
    audio.begin(toneGenerator, sample_rate);
}

void loop(){
    Serial.println("loop ");
}