cmake_minimum_required(VERSION 3.10)

project(Greenboy)

add_library(Greenboy
  src/greenboy/Gameboy.cpp
)

target_include_directories(Greenboy
  PUBLIC
    include
)

enable_testing()
