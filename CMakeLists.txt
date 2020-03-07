cmake_minimum_required(VERSION 3.10)

project(Greenboy)

option(GREENBOY_TESTS "Build the tests for the greenboy emulator" ON)
option(GREENBOY_COVERAGE "Generate coverage result" OFF)
option(GREENBOY_DOCS "Generate doxygen documentation" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(StandardProjectSettings)

list(APPEND GREENBOY_SOURCES 
  ${CMAKE_SOURCE_DIR}/src/greenboy/cpu.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/fetch_execute_cpu.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/gameboy.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/timing.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/video.cpp
)

add_library(Greenboy
  ${GREENBOY_SOURCES}
)

target_include_directories(Greenboy
  PUBLIC
    include
)

if(GREENBOY_DOCS)
  find_package(Doxygen
              REQUIRED dot
              OPTIONAL_COMPONENTS mscgen dia)
  set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
  set(DOXYGEN_WARN_NO_PARAMDOC YES)
  doxygen_add_docs(docs 
    "include/"
    "src/"
  )
endif()

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

add_custom_target(tidy 
  clang-tidy -p ${CMAKE_BINARY_DIR} ${GREENBOY_SOURCES}
)
