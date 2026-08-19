cmake_minimum_required(VERSION 3.16)

project(tinysketch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tinysketch
    cpp/src/main.cpp
    cpp/src/CountMinSketch.cpp
)

target_include_directories(tinysketch
    PRIVATE
        cpp/inc
)