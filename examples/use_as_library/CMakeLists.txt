cmake_minimum_required(VERSION 3.19.2)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare( arduino GIT_REPOSITORY https://github.com/pschatzmann/pico-arduino.git  GIT_TAG  main)
FetchContent_MakeAvailable(arduino)

project(use_as_library)

add_executable(use_as_library use_as_library.cpp)

# Pull in our arduino library
target_link_libraries(use_as_library arduino)
target_include_directories(use_as_library PUBLIC
    "${arduino_SOURCE_DIR}/Arduino"
    "${arduino_SOURCE_DIR}/Arduino/ArduinoCore-API/api" 
    "${arduino_SOURCE_DIR}/Arduino/ArduinoCore-Pico"
)
# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(use_as_library)

