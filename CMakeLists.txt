cmake_minimum_required(VERSION 3.10)

project(Greenboy)
option(GREENBOY_TESTS "Build the tests for the greenboy emulator" ON)
option(GREENBOY_COVERAGE "Generate coverage result" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_library(Greenboy
  src/greenboy/cpu.cpp
  src/greenboy/gameboy.cpp
  src/greenboy/timing.cpp
)

target_include_directories(Greenboy
  PUBLIC
    include
)

if(GREENBOY_COVERAGE)
  message(STATUS "Creating a coverage build")
  include(CodeCoverage)
endif()

if(GREENBOY_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
