cmake_minimum_required(VERSION 3.1)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project(vgl_test)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

output_flags("DEBUG" on)

add_definitions(-DUSE_LD)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(test
    "dnload.h"
    "test.cpp")
