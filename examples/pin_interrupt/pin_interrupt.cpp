#include "Arduino.h"

const int ledPin = LED_BUILTIN;
const int interruptPin = 15;
volatile bool state = LOW;

void handle_interrupt() {
    state = !state;
    digitalWrite(ledPin, state);
}

void setup() {
  Serial.begin();
  Serial.println("started...");

  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), handle_interrupt, CHANGE);
  Serial.println("handlers attached...");
}

void loop() {
  Serial.println("ping...");
  delay(5000);  
}

