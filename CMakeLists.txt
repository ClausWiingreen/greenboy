cmake_minimum_required(VERSION 3.10)

project(Greenboy)

option(GREENBOY_TESTS "Build the tests for the greenboy emulator" ON)
option(GREENBOY_COVERAGE "Generate coverage result" OFF)
option(GREENBOY_DOCS "Generate doxygen documentation" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(StandardProjectSettings)

add_library(greenboy_options INTERFACE)
target_compile_features(greenboy_options INTERFACE cxx_std_17)

add_library(greenboy_warnings INTERFACE)
include(CompilerWarnings)
set_warnings(greenboy_warnings)

list(APPEND GREENBOY_HEADERS
  ${CMAKE_SOURCE_DIR}/include/greenboy/cpu.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/fetch_execute_cpu.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/gameboy.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/instruction.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/memory_bus.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/opcode_translator.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/timing.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/types.hpp
  ${CMAKE_SOURCE_DIR}/include/greenboy/video.hpp
)

list(APPEND GREENBOY_SOURCES 
  ${CMAKE_SOURCE_DIR}/src/greenboy/cpu.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/fetch_execute_cpu.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/gameboy.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/instruction.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/memory_bus.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/opcode_translator.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/timing.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/types.cpp
  ${CMAKE_SOURCE_DIR}/src/greenboy/video.cpp
)

option(ENABLE_CPPCHECK "Enable the CppCheck static analyser" OFF)
if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK ${CPPCHECK}
      --suppress=missingInclude
      --enable=all
      --inconclusive
    )
  else()
    message(WARNING "CppCheck was not detected")
  endif()
endif()

option(ENABLE_CLANGTIDY "Enable the ClangTidy static analyser" OFF)
if(ENABLE_CLANGTIDY)
  find_program(CLANGTIDY clang-tidy)
  if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
  else()
    message(WARNING "ClangTidy was not detected")
  endif()
endif()

add_library(Greenboy
  ${GREENBOY_SOURCES}
  ${GREENBOY_HEADERS}
)

target_include_directories(Greenboy
  PUBLIC
    include
)

target_link_libraries(Greenboy
  PRIVATE
    greenboy_warnings
  PUBLIC
    greenboy_options
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

unset(CMAKE_CXX_CPPCHECK)
unset(CMAKE_CXX_CLANG_TIDY)

if(GREENBOY_TESTS)
  enable_testing()
  include(GoogleTest)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  add_subdirectory("external/gtest/")
  mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
  )
  add_subdirectory(tests)
endif()
