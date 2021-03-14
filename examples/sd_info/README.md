
## Overview
So far this has been just a Toy Project, but with the addition of SPI we can now take on more complex Arduino projects.

In order to verfify and test the __implementation of the Arduino SPI interface__. I added the SdFat library from Bill Greiman (https://github.com/greiman/SdFat) and just used the SdInfo example. SdFat Version 2 supports FAT16/FAT32 and exFAT SD cards.

You can find more examples in the SdFat/exampes directory.

The Arduino HardwareSPI interface using the Pico API and we use the following default pins

- spi0:  pinRx = 16; pinTx = 19; pinCS = 17; pinSCK = 18;
- spi1:  pinRx = 12; pinTx = 11; pinCS = 13; pinSCK = 10;

On the master MISO = pinRx and MOSI = pinTx
 
## Connections

 SD   | Pico              
------|-------------------
 CS   | SPI-CS0 (GPIO 17) 
 SCK  | SPI-SCK (GPIO 18) 
 MOSI | SPI-TX (GPIO 19)  
 MISO | SPI-RX (GPIO 16)  
 VCC  | VBUS (5V)         
 GND  | GND               

## Configuration
When I first tested the functionality, I wondered why the functionality did not work properly. It turned out that USE_SIMPLE_LITTLE_ENDIAN must not be set to 1!
