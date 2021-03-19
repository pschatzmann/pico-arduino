cmake_minimum_required(VERSION 3.12)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "webserver" )

## Use the adapted SdFat library
file(GLOB_RECURSE SDFAT_CPP CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/wifiesp-lib/src/*.cpp)
## define src and h
set(ARDUINO_SKETCH_SOURCE webserver.cpp ${SDFAT_CPP} )
set(ARDUINO_SKETCH_INCLUDES  ${CMAKE_CURRENT_SOURCE_DIR}/wifiesp-lib/src/)

set(ARDUINO_SKETCH_DEFINITIONS "-D_ESPLOGLEVEL_=4")

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

