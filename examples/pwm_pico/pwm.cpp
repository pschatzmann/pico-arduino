
#include "Arduino.h"
#include "PicoPWM.h"

//Initializing pwm
const int led_pin = GP15;
const int frequencyHz = 490;
const int maxValue = 255;
PicoPWM pwm(frequencyHz, maxValue);


void setup() {
  Serial.begin();
//  while(!Serial);
//  Logger.begin(Serial,PicoLogger::Debug);
  //Declaring LED pin as output
  pwm.begin(led_pin);
}

void loop() {
  //Fading the LED
  for(int i=0; i<maxValue; i++){
    Serial.println(i);
    pwm.write(led_pin, i);
    delay(5);
  }
  for(int i=maxValue; i>0; i--){
    Serial.println(i);
    pwm.write(led_pin, i);
    delay(5);
  }
}
