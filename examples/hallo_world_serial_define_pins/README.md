
We can also change the default pins also by re-defining them in the CMakeLists.txt

```
set(ARDUINO_SKETCH_DEFINITIONS "-DSERIAL2_TX=12" "-DSERIAL2_RX=13")
```

The name of all the availalable pins can be found in the [pins_arduino.h](../../Arduino/ArduinoCore-Pico/pins_arduino.h) file.

## Connections for the YP-05

Before making any connectins, make sure that the jumper is set to 3.3V !

 YP-05   | Pico              
---------|-------------------
 RX      | TX (GPIO 12) 
 TX      | RX (GPIO 13) 
 GND     | GND               

<img src="https://www.pschatzmann.ch/wp-content/uploads/2021/03/ftdi.jpg" alt="FTDI">

You can connect with any Termial Emulation using the following settings

- Speed: 115200
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- No Hardware flow control


