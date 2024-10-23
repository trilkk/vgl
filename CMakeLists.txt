cmake_minimum_required(VERSION 3.1)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project(vgl_test)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

find_boost("filesystem" "program_options" "wave")
find_freetype()
find_glew()
find_ogg()
find_sdl2()

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
    target_link_libraries("opus2raw" debug "${FREETYPE_LIBRARY_DEBUG}" optimized "${FREETYPE_LIBRARY}")
    target_link_libraries("opus2raw" debug "${GLEW_LIBRARY_DEBUG}" optimized "${GLEW_LIBRARY}")
    target_link_libraries("opus2raw" debug "${OGG_LIBRARY_DEBUG}" optimized "${OGG_LIBRARY}")
    target_link_libraries("opus2raw" debug "${SDL2_LIBRARY_DEBUG}" optimized "${SDL2_LIBRARY}")
else()
    target_link_libraries("opus2raw" "${BOOST_FILESYSTEM_LIBRARY}")
    target_link_libraries("opus2raw" "${BOOST_PROGRAM_OPTIONS_LIBRARY}")
    target_link_libraries("opus2raw" "${BOOST_WAVE_LIBRARY}")
    target_link_libraries("opus2raw" "${FREETYPE_LIBRARY}")
    target_link_libraries("opus2raw" "${GLEW_LIBRARY}")
    target_link_libraries("opus2raw" "${OGG_LIBRARY}")
    target_link_libraries("opus2raw" "${SDL2_LIBRARY}")
endif()
