
#include "Arduino.h"
#include "PicoPWM.h"

//Initializing LED Pin
int out_pin = GP2;
int in_pin = GP5;
const int maxValue = 255;

void setup() {
  Serial.begin();
  while(!Serial);
  Logger.begin(Serial, PicoLogger::Warning);

  pinMode(out_pin, OUTPUT);
  pinMode(in_pin, INPUT);

  Serial.println( PinFunction.isInput(in_pin) ? "OK" : "Error - wrong mode");
}

void loop() {
  //Fading the LED
  for(int i=0; i<maxValue; i++){
    Serial.print(i);
    analogWrite(out_pin, i);
    delay(5);
    Serial.print(" -> ");
    Serial.println(analogRead(in_pin));
  }
  for(int i=maxValue; i>0; i--){
    Serial.print(i);
    analogWrite(out_pin, i);
    delay(5);
    Serial.print(" -> ");
    Serial.println(analogRead(in_pin));
  }
}
