/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.
 modified 8 Nov 2013 by Scott Fitzgerald
 modified 21 March 2021 by Phil Schatzmann
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include "Arduino.h"
#include "Servo.h"

const int servo_pin = GP15;  // GPIO pin  number for PWM signal
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin();
  myservo.attach(servo_pin);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Serial.println(pos);
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    Serial.println(pos);
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
