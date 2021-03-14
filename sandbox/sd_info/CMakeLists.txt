cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "sd_bench" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download of SdFat from Guthub
FetchContent_Declare( sdfat GIT_REPOSITORY https://github.com/greiman/SdFat  GIT_TAG  master)
FetchContent_MakeAvailable(sdfat)

## Define all Source files incl SdFat
file(GLOB_RECURSE SDFAT_SRC CONFIGURE_DEPENDS "${sdfat_SOURCE_DIR}/src/*.cpp")
set(ARDUINO_SKETCH_SOURCE "bench.cpp" ${SDFAT_SRC})
set(ARDUINO_SKETCH_INCLUDES "${sdfat_SOURCE_DIR}/src/")
set(ARDUINO_SKETCH_DEFINITIONS "-DENABLE_ARDUINO_FEATURES")

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

