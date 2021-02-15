#include "Arduino.h"
#include "USB.h"

USBMidi midi = USBMidi::instance();
int note;
extern void midi_task();
extern void led_blinking_task();

void setup() {
    midi.begin();
    Serial1.println("setup ended");
}

void loop() {
    midi.noteOff(note);
    note = 58 + rand() % 14;
    Serial1.print("playing note ");
    Serial1.println(note);
    midi.noteOn(note);
    delay(1000);
}