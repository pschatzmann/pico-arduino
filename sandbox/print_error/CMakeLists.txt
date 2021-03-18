cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "print_error" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download of liquidcristal from Guthub
FetchContent_Declare( wifi_lib GIT_REPOSITORY https://github.com/jandrassy/WiFiEspAT.git GIT_TAG  master)
FetchContent_MakeAvailable(wifi_lib)

## Define all Source files 
file(GLOB_RECURSE LIB_SRC "${wifi_lib_SOURCE_DIR}/src/*.cpp")

set(ARDUINO_SKETCH_SOURCE  "print_error.cpp" ${LIB_SRC})
set(ARDUINO_SKETCH_INCLUDES  ${wifi_lib_SOURCE_DIR}/src)

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

