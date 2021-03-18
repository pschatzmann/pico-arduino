
The Pico does not support any wireless communication, but we can use a good old HC-05 to Bluetooth-enable the Pico.
The HC-05 uses simple serial communication and because we can use any GPIO pin we prefer Softwareserial:

## Connections 

 Device | Pico              
--------|------------------------
 VCC    | VBUS (5V)  
 GND    | GND 
 TX     | RX GP15
 RX     | TX GP14       


## Steps

To test the functionality, the sketch is just copying the input from one Serial device to the other.
We assume you use the default settings of the HC-05.

1. In Bluetooth menu connect your PC to the HC-05 device (using the password 1234)
2. Open a Terminal to connect to the Bluetooth port (Speed 9600, 8 Data bits, 1 Stop bit, no parity)
3. Open a Terminal and connect to the Pico
4. In the Bluetooth Terminal you can see ping messages
5. If you enter a string in the Bluetooth Terminal it will appear in the Pico Terminal
5. If you enter a string in the Pico Terminal it will appear in the Bluetooth Terminal


<img src="https://www.pschatzmann.ch/wp-content/uploads/2021/03/hc05.png" alt="hc-05">

