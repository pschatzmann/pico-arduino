Sound processing is a little bit challanging because we must make sure that we can process all data w/o causing any delays or otherwise we might loose some data and this leads to some audible stutter.

The Pico might be a good environment for sound processing because we can

- use the TimerAlarmRepeating to sample the data at a consistent rate
- use 1 processor to collect the data and the other to process it in parallel
- use the available synchronization functionality 

 ## Connections for the cjmcu-622 Microphone

 cjmcu-622 | Pico              
-----------|-------------------
 VCC       | 3.3V (OUT)         
 GND       | GND               
 OUT       | GPIO 26 (=ADC input 0)

