
#include "Arduino.h"
#include "PicoPWM.h"

//Initializing LED Pin
int led_pin = LED_BUILTIN;
const int maxValue = 255;

void setup() {
  Serial.begin();
  //while(!Serial);
  //Logger.begin(Serial,PicoLogger::Debug);

  //Declaring LED pin as output
  pinMode(led_pin, OUTPUT);
}

void loop() {
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
