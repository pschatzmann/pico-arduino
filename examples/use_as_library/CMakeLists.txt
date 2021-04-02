cmake_minimum_required(VERSION 3.19.2)
include(FetchContent)
FetchContent_Declare( arduino GIT_REPOSITORY https://github.com/pschatzmann/pico-arduino.git  GIT_TAG  master)
FetchContent_MakeAvailable(arduino)

add_executable(use_as_library use_as_library.cpp)

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(use_as_library arduino)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(use_as_library)

# add url via pico_set_program_url
example_auto_set_url(use_as_library)
