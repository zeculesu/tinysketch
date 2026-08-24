cmake_minimum_required(VERSION 3.16)

project(tinysketch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Verilator / hardware model
find_package(verilator REQUIRED)
add_library(tinysketch-verilator STATIC)
verilate(tinysketch-verilator
    SOURCES
        ${CMAKE_SOURCE_DIR}/rtl/TinySketch.sv
    TOP_MODULE
        TinySketch
)

# Software implementation
add_library(tinysketch-lib STATIC
    cpp/src/CountMinSketch.cpp
)
target_include_directories(tinysketch-lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/cpp/inc
)

# Hardware C++ wrapper
add_library(tinysketch-hardware STATIC
    cpp/src/TinySketchHardware.cpp
)
target_include_directories(tinysketch-hardware
    PUBLIC
        ${CMAKE_SOURCE_DIR}/cpp/inc
)
target_link_libraries(tinysketch-hardware
    PUBLIC
        tinysketch-verilator
)

enable_testing()
add_subdirectory(test)
