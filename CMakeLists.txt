cmake_minimum_required(VERSION 3.10)

project(Greenboy)
option(GREENBOY_TESTS "Build the tests for the greenboy emulator" ON)
option(GREENBOY_COVERAGE "Generate coverage result" OFF)

add_library(Greenboy
  src/greenboy/cpu.cpp
  src/greenboy/gameboy.cpp
  src/greenboy/timing.cpp
)

target_include_directories(Greenboy
  PUBLIC
    include
)

if(GREENBOY_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
