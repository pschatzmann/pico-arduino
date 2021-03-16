cmake_minimum_required(VERSION 3.12)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "mic_to_sd" )

## Download of SdFat from Guthub
FetchContent_Declare( sdfat GIT_REPOSITORY https://github.com/greiman/SdFat  GIT_TAG  master)
FetchContent_MakeAvailable(sdfat)
# Replace SdFatConfig.h with our version
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/SdFatConfig.h DESTINATION ${sdfat_SOURCE_DIR}/src/)

file(GLOB_RECURSE SDFAT_SRC CONFIGURE_DEPENDS "${sdfat_SOURCE_DIR}/src/*.cpp")
set(ARDUINO_SKETCH_SOURCE mic_to_sd.cpp ${SDFAT_SRC} )
set(ARDUINO_SKETCH_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${sdfat_SOURCE_DIR}/src/)

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

