cmake_minimum_required(VERSION 3.12)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_SOURCE "sd_info.cpp" )
set(ARDUINO_SKETCH_NAME "sd_info" )

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

