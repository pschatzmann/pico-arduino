
## Overview
 
In order to test the I2C implementation, I used a HW-290 3-axis Gyro + 3-axis Acceleration + 3-axis Magnetic Field + Air Pressure Sensor.
In Pico Arduino we have predefined the following objects:

 - PicoHardwareI2C Wire(i2c0, 160, GP12, GP13);  
 - PicoHardwareI2C Wire1(i2c1, 160, GP14, GP15);  


We access the HW-290 with the help of the https://github.com/ElectronicCats/mpu6050.git Arduino library.

## Connections 

The advantage of I2c is that it only needs 2 connections: a data and a clock line:

 HW-290 | Pico              
--------|------------------------
 VCC_IN | 3.3V (OUT) or VBUS (5V)  
 3.3V   | -
 GND    | GND 
 SCL    | GP13 
 SDA    | GP12        
 FSYNC  | -
 INTA   | -              

The module is working both with 3.3 and with 5V!

<img src="https://www.pschatzmann.ch/wp-content/uploads/2021/03/hw-290gy-87_-_2.png" alt="hw290">


