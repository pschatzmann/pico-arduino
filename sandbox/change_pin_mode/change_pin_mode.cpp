#include "Arduino.h"

TimerAlarmRepeating timer;
const int led_pin = LED_BUILTIN;
bool is_fade;
const int maxValue = 255;


void blink() {
    Serial.println("blink...");
    digitalWrite(led_pin, true);
    delay(1000);  
    digitalWrite(led_pin, false);
    delay(1000);  
}


void fade(){
  Serial.println("fade...");
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




void setup() {
    Serial.begin();
    while(!Serial);
    Logger.begin(Serial,PicoLogger::Debug);
    Serial.println("setup...");
    pinMode(led_pin, OUTPUT);
}

void loop(){
    blink();
    fade();
}


