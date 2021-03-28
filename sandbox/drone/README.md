
The multiwii project is one of the most important Arduino based drone projects. Unfortunatly it
- does writes directly to ports instead of using the Arduino API
- uses some assembler
- calls directly different avr specifc funtionality (avr/eeprom.h, avr/io.h)

Therefore I discarded it and used the following (more managable) Arduino drone project insted:

- https://github.com/liourej/CodeDroneDIY.git 

Here as well, I needed to make some small adjustment and replace the MotorsSpeedControl.cpp with a implementation which uses the PicoPWM class
