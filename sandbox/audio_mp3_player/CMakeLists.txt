cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "mp3_player" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download Libraries from Guthub
FetchContent_Declare( rp2040_i2s GIT_REPOSITORY https://github.com/pschatzmann/rp2040_i2s.git GIT_TAG  main)
FetchContent_MakeAvailable(rp2040_i2s)

FetchContent_Declare( libhelix GIT_REPOSITORY https://github.com/pschatzmann/arduino-libhelix  GIT_TAG  main)
FetchContent_MakeAvailable(libhelix)

FetchContent_Declare( audio_tools GIT_REPOSITORY https://github.com/pschatzmann/audio_tools.git GIT_TAG main)
FetchContent_MakeAvailable(audio_tools)

FetchContent_Declare( sdfat GIT_REPOSITORY https://github.com/greiman/SdFat  GIT_TAG  master)
FetchContent_MakeAvailable(sdfat)

ADD_DEFINITIONS(-DUSE_HELIX)
ADD_DEFINITIONS(-DUSE_SDFAT)


## Define all Source files incl SdFat
file(GLOB_RECURSE SDFAT_SRC CONFIGURE_DEPENDS "${sdfat_SOURCE_DIR}/src/*.cpp")
set(ARDUINO_SKETCH_SOURCE  "mp3_player.cpp" ${SDFAT_SRC})
set(ARDUINO_SKETCH_INCLUDES  "${sdfat_SOURCE_DIR}/src/")
set(ARDUINO_SKETCH_DEFINITIONS "-DENABLE_ARDUINO_FEATURES")


include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

