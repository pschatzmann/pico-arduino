## Raspberry PI Pico - Arduino API

Recently I was receiving my Raspberry PI Pico and I had the opportunity to play around with it. Actually most of the time I spent reading the Manuals...
The conclusion: interesting - but a little bit too complicated. So I was quite happy to read that Arduino plans to add an official support for this Microcontroller. 

However I did not want to wait for the official support to be available, so as a temporary solution, I decided to make my own Arduino API availale as a separate project. 

## Design goals

- Provide a full implementation of the Arduino API 
- Provide additional easy to use C++ classes for the functionaity which is specific to the Pico
- Keep the pico specific build process using cmake
- Support of existing Arduino Libraries
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
make install
```

You can find the resulting library in the pico-arduino/lib directory.

## Examples

Details can be found in the [examples directory](examples)

## Documentation

The generated [Class Documentation](https://pschatzmann.github.io/pico-arduino/doc/html/) can be found in the doc folder

## Pinout

It helps to know the functions of the pins. Here is a copy of the pinout provided by the official [Raspberry Pi Pico Documentation](https://www.raspberrypi.org/documentation/pico/getting-started/):

<img src="https://www.raspberrypi.org/documentation/pico/getting-started/static/15243f1ffd3b8ee646a1708bf4c0e866/Pico-R3-Pinout.svg" alt="Raspberry Pi Pico pin out diagram">

If a method is requiring a pin, you can provide the GPIO number as integer or use the corresponding GP name as given in the sheet above.

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

## Project Status / Change History

[The project status and change history can be found here](history.md)
