## Raspberry PI Pico - Arduino API

On Friday I was receiving my Raspberry PI Pico and I had the opportunity to play around with it. Actually most of the time I spent reading the Manuals...
The conclusion: interesting - but a little bit too complicated - So I was quite happy to read that Arduino plans to add an official support for this Microcontroller. 

However I did not want to wait for the official support to be available, so as a temporary solution, I decided to make my own Arduino API availale as a separate project. 

## Design goals

- Provide a full implementation of the Arduino API
- Provide additioal easy to use C++ classes for the functionaity which is specific to the Pico
- Keep the pico specific build process using cmake
- Provide examples 

## Installation

1. First you need to install the Pico C Framwork. Please [follow the instructions](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf) and do not forget to define the PICO_SDK_PATH environment variable.

2. Clone this project with 

```
git clone https://github.com/pschatzmann/pico-arduino
```
3. Define the environment PICO_SDK_ARDUINO_PATH variable to point to the project location.
4. Build the arduino library (optional). 
The project already contains the compiled library. However I would recommend to rebuild it from the source.

To build the arduino library go to the pico-arduino/Arduino subdirectory 
```
cd pico-arduino/Arduino
```
and execute the steps below:
```
mkdir build
cd build
cmake ..
make
```



You can find the resulting library in the pico-arduino/lib directory.


## Examples

Let's have a look at the blink example. It conisists of the source code blink.cpp
```
#include "Arduino.h"

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```
This looks like a standard Arduino sketch. Please note the following differences however:
- The sketch file name must end with .cpp and not .ino
- The ```#include "Arduino.h"``` is mandatory
- You need to provide a CMakeLists.txt file

To build the example go to the pico-arduino/examples/blink directory and execute the build steps:
```
mkdir build
cd build
cmake ..
make
```
The result can be found in the arduino/examples/blink/build directory.

Here is the list of the provided examples

- [hallo_world](hallo_world)
- [blink](examples/blink)
- [tone](tone)

The Pico has 2 processors, so here are the examples which use this Pico specific functionality:

- [multi_processor](multi_processor) 
- [multi_processor_fifo](multi_processor_fifo) 
- [queue](queue)
- [semaphore](semaphore) .
And finally some other Pico specific functionality:

- [blink_timer](blink_timer)

## Documentation

The generated [Class documentation](/doc/html/index.html) can be found in the doc folder

## Pinout

It helps to know the functions of the pins. Here is a copy of the pinout provided by the official [Raspberry Pi Pico Documentation](https://www.raspberrypi.org/documentation/pico/getting-started/):

<img src="https://www.raspberrypi.org/documentation/pico/getting-started/static/15243f1ffd3b8ee646a1708bf4c0e866/Pico-R3-Pinout.svg" alt="Raspberry Pi Pico pin out diagram">

If a method is requiring a pin you can provide the GPIO number as integer or use the corresponding GP name as given in the sheet above.

The following calls are all identical:

```
  digitalWrite(25, HIGH);                      // turn the LED on using integer
  digitalWrite(GP25, HIGH);                    // the same with GP25 define
  digitalWrite(LED_BUILTIN, HIGH);             // the same with Arduino defines 
  digitalWrite(PICO_DEFAULT_LED_PIN, HIGH);    // the same with Pico defines 
```

## Deploying a compiled Sketch

Deployment is very easy - just press the button on your Pico while plugging it to your computer via USB. This will automatically mount the Pico as external drive (RPI-RP2).
Finally you just need to copy the generated uf2 file from your build directory to the drive.

That's it!

## Project Status

- Basic Arduino API (e.g. String, Serial...) - done
- digital input/output API - done
- analog input/output API (PWM) - done
- Multiprocessor support: queue, semaphore - done
- tone - in progress
- PIO API class - in progress
- Arduino SPI API - open
- Arduino I2C API - open
- I2S API using PIO - open
- Software Serial using PIO - open



