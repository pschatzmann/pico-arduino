
## Overview

A I2S test application which procduces a sine wave output. It uses the [rp2040-i2s library](https://github.com/pschatzmann/rp2040-i2s)

## Connections 

You need to connect an external DAC module with the Pico:

 DAC    | Pico              
--------|------------------------
 VCC    | 5.5V (OUT) 
 GND    | GND 
 BCLK   | GPIO26 
 LRCLK  | GPIO27
 DOUT   | GPIO28


The LRCLK can not be defined separately and is BCLK+1

