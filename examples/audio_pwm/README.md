# Decoding a WAV file

In this example we decode a WAV file into RAW output and send it to a PWM pins (on a Raspberry Pico or ESP32)
The WAV file has been down sampled with the help of __Audacity__ and then converted into an array with __xxd__. 

# Processing Logic

The Sketch implements the following audio pipline:

MemoryStream -> AudioOutputStream -> WAVDecoder -> AudioPWM

The output pin is GPIO2


# Dependencies

- [arduino-audio-tools]()