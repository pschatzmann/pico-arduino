cmake_minimum_required(VERSION 3.11)
include(FetchContent)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_SOURCE "audio_pwm.cpp" )
set(ARDUINO_SKETCH_NAME "audio_pwm" )
set(ARDUINO_SKETCH_LIB "arduino-audio-tools" )

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}")

# make arduino-audio-tools available
FetchContent_Declare( arduino-audio-tools  GIT_REPOSITORY "https://github.com/pschatzmann/arduino-audio-tools.git" GIT_TAG main )
FetchContent_MakeAvailable(arduino-audio-tools)

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

