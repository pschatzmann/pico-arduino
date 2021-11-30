cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "i2s_test" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download Libraries from Guthub
FetchContent_Declare( rp2040-i2s GIT_REPOSITORY https://github.com/pschatzmann/rp2040-i2s.git GIT_TAG main)
FetchContent_MakeAvailable(rp2040-i2s)
# add_subdirectory("/Users/pschatzmann/Documents/Arduino/libraries/rp2040-i2s" rp2040-i2s-bin)

## Define all Source files incl rp2040-i2s
set(ARDUINO_SKETCH_SOURCE  "i2s_test.cpp" )
set(ARDUINO_SKETCH_LIB rp2040-i2s)
set(ARDUINO_SKETCH_DEFINITIONS "-DARDUINO_ARCH_RP2040")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake" )

