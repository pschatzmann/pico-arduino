#include "Arduino.h"
#include "hardware/watchdog.h"


void setup() {
    Serial.begin();
 
    if (watchdog_caused_reboot()) {
        Serial.println("Rebooted by Watchdog!");
        return;
    } else {
        Serial.println("Clean boot");
    }
 
    // Enable the watchdog, requiring the watchdog to be updated every 100ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(100, 1);
}

void loop() {
    for (int i = 0; i < 10; i++) {
        Serial.print("Updating watchdog ");
        Serial.println(i);
        watchdog_update();
    }
 
    // Wait in an infinite loop and don't update the watchdog so it reboots us
    Serial.println("Waiting to be rebooted by watchdog");
    while(1);
}
