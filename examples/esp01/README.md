
The major weak spot of the Pico compared to the ESP32 is the missing Wifi functionality.
The ESP01 was a pain with the regular 5V Arduino Boards because of the 3.3V logic - but for the Pico it seems to be a perfect fit.
The examples are working both by using the regular HardwareSerial or with the SoftwareSerial.

## WiFiEsp 

Fortunately we can use a ESP01 module to get Wifi together with the following library: https://github.com/bportaluri/WiFiEsp.git. 

I included a slightly adapted version of the [WiFiEsp](https://github.com/bportaluri/WiFiEsp) library. The original version is not compatible with the latest version of the Arduino framwork, so I needed to adapt it to resolve the compile errors. 

- renamed RingBuffer to RingBufferESP

## Connections 

| ESP01  | Pico              
|--------|-----------
|  3V3   | 3V3 (OUT) 
|  RST   |  
|  EN    | 3V3 
|  TX    | RX GP9 
|  RX    | TX GP8 
|  IO0   |  
|  IO2   |  
|  GND   | GND 

<img src="https://www.robotbanao.com/wp-content/uploads/2020/02/esp11.png" alt="ESP01">


