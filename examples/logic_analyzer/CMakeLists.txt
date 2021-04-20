cmake_minimum_required(VERSION 3.19.2)

project(install-logic-analyzer)

include(ExternalProject)

ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/pschatzmann/logic-analyzer.git
  GIT_TAG           main
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/src"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/bin"
)

