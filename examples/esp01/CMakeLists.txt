cmake_minimum_required(VERSION 3.12)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "webserver" )

## Download of Arduino Library from Guthub
FetchContent_Declare( wifi_lib GIT_REPOSITORY https://github.com/bportaluri/WiFiEsp.git GIT_TAG  master)
FetchContent_MakeAvailable(wifi_lib)


## Use the adapted SdFat library
file(GLOB_RECURSE WIFI_CPP CONFIGURE_DEPENDS ${wifi_lib_SOURCE_DIR}/src/*.cpp)
## define src and h
set(ARDUINO_SKETCH_SOURCE webserver.cpp ${WIFI_CPP} )
set(ARDUINO_SKETCH_INCLUDES  ${wifi_lib_SOURCE_DIR}/src)

set(ARDUINO_SKETCH_DEFINITIONS "-D_ESPLOGLEVEL_=4" "-DRINGBUFFER_TYPEDEF_OFF")

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

