cmake_minimum_required(VERSION 3.10)

project(Greenboy)

add_library(Greenboy
  src/greenboy/gameboy.cpp
)

target_include_directories(Greenboy
  PUBLIC
    include
)

option(GREENBOY_TESTS "Build the tests for the greenboy emulator" ON)
if(GREENBOY_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
