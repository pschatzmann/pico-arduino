#include "Arduino.h"
#include "USB.h"
#include "AudioTools/AudioTools.h"

USBAudio audio;
SineWaveGenerator<int16_t> toneGemerator(32000);
int sample_rate=AUDIO_SAMPLE_RATE;
int frequency=220;

void setup() {
    toneGemerator.begin(sample_rate, frequency);
    audio.begin(toneGemerator, 1);
}

void loop(){

}