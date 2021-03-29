#pragma once

#include "hardware/watchdog.h"

#define WDTO_1S 1000
#define WDTO_15MS  15
#define WDTO_30MS  30
#define WDTO_60MS  60
#define WDTO_120MS  120
#define WDTO_250MS  250
#define WDTO_500MS  500
#define WDTO_1S   1000
#define WDTO_2S   2000
#define WDTO_4S   4000
#define WDTO_8S   8000


inline void wdt_reset(){
    watchdog_update();
}

inline void wdt_enable(int delay_ms ){
#ifndef NO_WATCHDOG
    watchdog_enable(delay_ms, false);
#endif
}




