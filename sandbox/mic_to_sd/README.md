Sound processing is a little bit challanging because we must make sure that we can process all data w/o causing any delays or otherwise we might loose some data and this leads to some audible stutter.

The Pico might be a good environment for sound processing because we can

- use the TimerAlarmRepeating to sample the data at a consistent rate
- use 1 processor to collect the data and the other to process it in parallel
- use the available synchronization functionality 

 ## Processing Logic

- We preallocate a fixed number of buffers. 
- The sketch is sampling the microphone with the help of the TimerAlarmRepeating into a available buffer. 
- On the second processor we just write the data to the SD card as soon as the buffer has been filled up. 
- After the data has been written the buffer is make available to the microphone again.
- New files are generated every minute with an increasing number in the name.
- We also expect a samping rate of 44100 Hz. To be able to verify if the timer is working as expected, we just print the measured sampling rate every 10 seconds (in the loop()).

This basic logic can be made available __with only 200 lines of code__!

 ## Connections for the cjmcu-622 Microphone

 cjmcu-622 | Pico              
-----------|-------------------
 VCC       | 3.3V (OUT)         
 GND       | GND               
 OUT       | GPIO 26 (=ADC input 0)

<img src="https://www.pschatzmann.ch/wp-content/uploads/2021/03/cjmcu-622.jpeg" alt="cjmc-622">


 ## Connections for the SD Module

 SD   | Pico              
------|-------------------
 CS   | SPI-CS0 (GPIO 17) 
 SCK  | SPI-SCK (GPIO 18) 
 MOSI | SPI-TX (GPIO 19)  
 MISO | SPI-RX (GPIO 16)  
 VCC  | VBUS (5V)         
 GND  | GND               

<img src="https://www.pschatzmann.ch/wp-content/uploads/2020/12/SD.jpeg" alt="SD Module">


