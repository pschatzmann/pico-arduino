#include "Arduino.h"


// functionality to be measured
void test() {
    for(int j=0;j<100000;j++){
        Serial.write('-');
    }
}

// take and print the time
void measure() {
    long start = millis();

    // run method 
    test();

    Serial.println();
    Serial.print("runtime (ms): ");
    Serial.println((millis()-start));
}

void setup(){
    // Serial is mapped to the default Pico output. 
    Serial.begin();
    // Wait for terminal connection
    while(!Serial){
        delay(100);
    }

}

void loop(){
    measure();
}