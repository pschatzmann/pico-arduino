cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "sine_wave" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download of Arduino Library from Guthub
FetchContent_Declare( mozzi GIT_REPOSITORY https://github.com/pschatzmann/Mozzi.git GIT_TAG  master)
FetchContent_MakeAvailable(mozzi)
file(GLOB MOZZI_CPP CONFIGURE_DEPENDS "${mozzi_SOURCE_DIR}/*.cpp" )

## Define all Source files 
set(ARDUINO_SKETCH_SOURCE  "sine_wave.cpp" ${MOZZI_CPP})
set(ARDUINO_SKETCH_INCLUDES  "${mozzi_SOURCE_DIR}")
set(ARDUINO_SKETCH_DEFINITIONS "-DARDUINO_ARCH_RP2040")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

