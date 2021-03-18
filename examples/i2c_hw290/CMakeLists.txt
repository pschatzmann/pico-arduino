cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "i2c_hw290" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download of hw290 Arduino library from Guthub
FetchContent_Declare( hw290 GIT_REPOSITORY https://github.com/ElectronicCats/mpu6050.git  GIT_TAG  master)
FetchContent_MakeAvailable(hw290)

## Define all Source files incl hw290
file(GLOB HW290_SRC "${hw290_SOURCE_DIR}/src/*.cpp")
set(ARDUINO_SKETCH_SOURCE  "i2c_hw290.cpp" ${HW290_SRC})
set(ARDUINO_SKETCH_INCLUDES  "${hw290_SOURCE_DIR}/src/")

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")
