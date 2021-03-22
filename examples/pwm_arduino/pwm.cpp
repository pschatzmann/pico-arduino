
#include "Arduino.h"
#include "PicoPWM.h"

//Initializing LED Pin
int led_pin = GP15;

void setup() {
//  Serial.begin();
//  while(!Serial);
//  Logger.begin(Serial,PicoLogger::Debug);

  //Declaring LED pin as output
  pinMode(led_pin, OUTPUT);
}

void loop() {
  //Fading the LED
  for(int i=0; i<255; i++){
    analogWrite(led_pin, i);
    delay(5);
  }
  for(int i=255; i>0; i--){
    analogWrite(led_pin, i);
    delay(5);
  }
}
