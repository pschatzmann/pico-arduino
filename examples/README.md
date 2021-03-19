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

- [blink](blink)
- [hallo_world](hallo_world)
- [tone](tone_melody)
- [Using an Arduino library](example_with_arduino_library)
- [SPI with SdFat](sd_info)
- [I2C with HW290](i2c_hw290)
- [hallo_world_serial](hallo_world_serial)
- [Bluetooth with a HC-05](hc_05)
- [Wifi with a ESP01](esp01)

The Pico has 2 processors, so here are the examples which use this Pico specific functionality:

- [multi_processor](multi_processor) 
- [multi_processor_fifo](multi_processor_fifo) 
- [queue](queue)
- [semaphore](semaphore) 

Here is some other Pico specific functionality:

- [blink_timer](blink_timer)
- [software_serial](software_serial)

