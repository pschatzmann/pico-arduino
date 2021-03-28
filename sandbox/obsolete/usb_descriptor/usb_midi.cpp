#include "Arduino.h"
#include "USB.h"

USBMidi midi = USBMidi::instance();
int note;

void setup() {
    Serial.begin();
    midi.begin();
    Serial.println("setup ended");
}

void loop() {
    midi.noteOff(note);
    note = 58 + rand() % 14;
    Serial.print("playing note ");
    Serial.println(note);
    midi.noteOn(note);
    delay(1000);
}