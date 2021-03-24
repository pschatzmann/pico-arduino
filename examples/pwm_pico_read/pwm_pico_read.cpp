
#include "Arduino.h"
#include "PicoPWM.h"

//Initializing pwm
int out_pin = GP2;
int in_pin = GP5;
const int frequencyHz = 490;
const int maxValue = 255;
PicoPWM pwm(frequencyHz, maxValue);


void setup() {
  Serial.begin();
  pwm.begin(out_pin, OUTPUT);
  pwm.begin(in_pin, INPUT);
}

void loop() {
  for(int i=0; i<maxValue; i++){
    Serial.print(i);
    pwm.write(out_pin, i);
    delay(5);
    Serial.print(" -> ");
    Serial.println(pwm.read(in_pin));
  }
  for(int i=maxValue; i>0; i--){
    Serial.print(i);
    pwm.write(out_pin, i);
    delay(5);
    Serial.print(" -> ");
    Serial.println(pwm.read(in_pin));
  }

}
