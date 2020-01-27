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
  append_coverage_compiler_flags()
endif()

if(GREENBOY_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory("external/gtest/")
  mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
  )
  add_subdirectory(tests)
endif()
