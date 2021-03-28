#pragma once

#include "hardware/watchdog.h"

#define WDTO_1S 1000

inline void wdt_reset(){
    watchdog_update();
}

inline void wdt_enable(int delay_ms ){
    watchdog_enable(delay_ms, false);
}

