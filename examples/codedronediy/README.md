
Most Arduino Drone projects are optimized for AVR processors by writing directly to registers and use other AVR specific functionality (e.g. timers interrupts). The multiwii project is a good example of this and it would take quite some effort because this logic is distributed all over the place.

I adapted the [CodeDroneDIY](https://github.com/liourej/CodeDroneDIY.git) because I only needed to replace the MotorsSpeedControl.cpp with a implementation which uses my [PicoPWM](https://pschatzmann.github.io/pico-arduino/doc/html/classpico__arduino_1_1_pico_p_w_m.html) class. 
