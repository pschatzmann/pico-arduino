#include "Arduino.h"
#include "USB/USBAudio.h"
#include "AudioTools/AudioTools.h"

USBAudio audio;
SineWaveGenerator<int16_t> toneGemerator;
int sample_rate=AUDIO_SAMPLE_RATE;
int frequency=220;

void setup() {
    toneGemerator.begin(sample_rate, frequency);
    audio.begin(toneGemerator);
}

void loop(){

}