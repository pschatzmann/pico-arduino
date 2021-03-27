#include "Arduino.h"

// Include the string library
#include <string>

// Create a string variable
const String arduinoString = "I am an Arduino String";
const std::string cppString = "I am a c++ string";

void setup(){
    Serial.begin();
    while(!Serial);

    Serial.println(arduinoString);
    Serial.println(cppString.c_str());

}

void loop(){
}