
The multiwii project is one of the most important Arduino based drone projects. Unfortunatly it
- does writes directly to ports instead of using the Arduino APP
- uses some assembler
- call directly avr specifc funtionality (avr/eeprom.h, avr/io.h)

Therefore we the following Arduino compliant drone project insted:

- https://github.com/liourej/CodeDroneDIY.git 

Here as well, I needed to make some small adjustment

- Replaced the MotorsSpeedControl.cpp with a implementation which uses the PicoPWM class
- Provided implementation for missing functions used from avr/wdt.h