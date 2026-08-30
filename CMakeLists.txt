cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

project(tinysketch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# RTL configuration (can be overridden with -DRTL_ROWS=... etc.)
set(RTL_ROWS 4 CACHE STRING "Number of rows")
set(RTL_COLS 256 CACHE STRING "Number of columns")
set(RTL_WIDTH 32 CACHE STRING "Counter width")
set(RTL_SOURCE TinySketch.sv CACHE STRING "RTL source file")

# C++ test configuration (mirrors RTL config)
set(TEST_ROWS ${RTL_ROWS})
set(TEST_COLS ${RTL_COLS})
set(TEST_WIDTH ${RTL_WIDTH})

configure_file(
    ${CMAKE_SOURCE_DIR}/rtl/${RTL_SOURCE}
    ${CMAKE_CURRENT_BINARY_DIR}/TinySketch.sv
    COPYONLY
)

# Verilator / hardware model
find_package(verilator REQUIRED)
add_library(tinysketch-verilator STATIC)
verilate(tinysketch-verilator
    SOURCES
        ${CMAKE_SOURCE_DIR}/rtl/${RTL_SOURCE}
    TOP_MODULE
        TinySketch
)

# Software implementation
add_library(tinysketch-lib INTERFACE)
target_include_directories(tinysketch-lib
    INTERFACE
        ${CMAKE_SOURCE_DIR}/cpp/inc
)

# Hardware C++ wrapper
add_library(tinysketch-hardware INTERFACE)
target_include_directories(tinysketch-hardware
    INTERFACE
        ${CMAKE_SOURCE_DIR}/cpp/inc
)
target_link_libraries(tinysketch-hardware
    INTERFACE
        tinysketch-verilator
)

enable_testing()
add_subdirectory(test)
