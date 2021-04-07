#include "Arduino.h"

const int led_pin = LED_BUILTIN;

void setup() {
    Serial.begin();
    //Logger.begin(Serial,PicoLogger::Debug);
    Serial.println("setup...");
    pinMode(led_pin, OUTPUT);
}


void blink() {
    Serial.println("blink...");
    digitalWrite(led_pin, true);
    delay(1000);  
    digitalWrite(led_pin, false);
    delay(1000);  
}


void fade(){
  Serial.println("fade...");
  const int maxValue = 255;
  //Fading the LED
  for(int i=0; i<maxValue; i++){
    Serial.println(i);
    analogWrite(led_pin, i);
    delay(5);
  }
  for(int i=maxValue; i>0; i--){
    Serial.println(i);
    analogWrite(led_pin, i);
    delay(5);
  }
}


void loop(){
    blink();
    fade();
}


