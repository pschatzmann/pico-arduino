## Project Status

- Basic Arduino API (e.g. String, Serial...) - completed
- digital input/output API - completed
- analog input/output API (PWM) - completed
- Multiprocessor support: queue, semaphore - completed
- tone - completed
- Arduino SPI API - completed
- PIO API class - completed
- Arduino I2C API - completed
- Software Serial using PIO - completed
- I2S API using PIO - open

## Change History

- __v0.1__ Initial Version with Baisic Functionality
- __v0.2__ SPI Support & error corrections
- __v0.3__ I2C Support & error corrections
- __v0.4__ Software Serial & error corrections
- __v0.5__ Error corrections, cleanup and more examples
     - HardwareSerial: Corrected slow print and println
     - analogWrite correction provided by Duke Dylan
     - Moved examples documentation into examples folder
     - Examples for HC-05 and ESP01
     - This release contains the following __Breaking Changes__
        - Changed signature of SoftwareSerial.begin() by switching the tx and rx sequence to be consistent with HardwareSerial
        - Removed unnecessary subfolders in ArduinoCore-Pico
- __v0.6__ PWM, error corrections and additional examples.
     - Provide easy to use PicoPWM class which is used by Arduino analogWrite
     - Implement Servo class with examples
     - Example how to change default pins in cmake 
     - provide termperature() and temperatureF() method to read the built in temperature sensor
     - This release contains the following __Breaking Changes__
          - Removed libarduino: all sources are complied as part of the sketch now, so that we can overwrite default defines 
- __v0.7__ PWM reading / DMA / pico_arduino Namespace
     - Implemented Reading of PWM 
     - Support for PWM in analogRead()
     - Added documentation on how to override the predefined framework #defines 
     - Redesign of internal PicoPinFunction: getting rid of ugly switch case.
     - Examples for reading PWM
     - Example for watchdog
     - DMA support with examples
     - Implementation of namespace using pico_arudino
     - Drone example
- __v0.8__ More examples
     - Tensorflow
