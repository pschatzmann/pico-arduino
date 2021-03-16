cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "liquidcristal_hallo_world" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download of liquidcristal from Guthub
FetchContent_Declare( liquidcristal GIT_REPOSITORY https://github.com/johnrickman/LiquidCrystal_I2C.git GIT_TAG  master)
FetchContent_MakeAvailable(liquidcristal)

## Define all Source files 
set(ARDUINO_SKETCH_SOURCE  "hallo_world.cpp" "${liquidcristal_SOURCE_DIR}/LiquidCrystal_I2C.cpp")
set(ARDUINO_SKETCH_INCLUDES  "${liquidcristal_SOURCE_DIR}")

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

