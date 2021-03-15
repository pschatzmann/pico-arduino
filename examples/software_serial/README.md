## Overview
SoftwareSerial is using the PIO and you can set up any available GPIO pin as RX and TX.
To test the functionality I am using a YP-05 FTDI 

## Connections for the YP-05

Before making any connectins, make sure that the jumper is set to 3.3V !

 YP-05   | Pico              
---------|-------------------
 RX      | TX (GPIO 21) 
 TX      | RX (GPIO 22) 
 GND     | GND               

<img src="https://www.pschatzmann.ch/wp-content/uploads/2021/03/ftdi.jpg" alt="FTDI">

You can connect with any Termial Emulation using the following settings

- Speed: 115200
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- No Hardware flow control