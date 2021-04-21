cmake_minimum_required(VERSION 3.1)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project(vgl_test)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

find_boost("filesystem" "program_options")
find_ogg()

output_flags("DEBUG" on)

add_definitions(-DUSE_LD)

include_directories("${PROJECT_SOURCE_DIR}")

set(VGL_ROOT "${PROJECT_SOURCE_DIR}")
include("${VGL_ROOT}/filelist.cmake")

add_executable("test"
    "dnload.h"
    "test.cpp")

add_executable("opus2raw"
    "opus2raw.cpp"
    ${VGL_HEADERS}
    ${VGL_SOURCES})
if(MSVC)
    target_link_libraries("opus2raw" debug "${OGG_LIBRARY_DEBUG}" optimized "${OGG_LIBRARY}")
else()
    target_link_libraries("opus2raw" "${BOOST_FILESYSTEM_LIBRARY}")
    target_link_libraries("opus2raw" "${BOOST_PROGRAM_OPTIONS_LIBRARY}")
    target_link_libraries("opus2raw" "${OGG_LIBRARY}")
endif()
