function(check_c_flag flag var)
    include(CheckCCompilerFlag)
    set(output_message "-- C compiler flag ${flag} - ")
    check_c_compiler_flag("${flag}" ${var})
    if(${${var}})
        if(2 LESS ${ARGC})
            if("DEBUG" STREQUAL "${ARGV2}")
                string(STRIP "${CMAKE_C_FLAGS_DEBUG} ${flag}" NEW_FLAGS)
                set(CMAKE_C_FLAGS_DEBUG "${NEW_FLAGS}" PARENT_SCOPE)
            elseif("RELEASE" STREQUAL "${ARGV2}")
                string(STRIP "${CMAKE_C_FLAGS_RELEASE} ${flag}" NEW_FLAGS)
                set(CMAKE_C_FLAGS_RELEASE "${NEW_FLAGS}" PARENT_SCOPE)
            else()
                message(SEND_ERROR "unknown build type ${ARGV1}")
            endif()
        else()
            string(STRIP "${CMAKE_C_FLAGS} ${flag}" NEW_FLAGS)
            set(CMAKE_C_FLAGS "${NEW_FLAGS}" PARENT_SCOPE)
        endif()
        set(${var} TRUE PARENT_SCOPE)
        message("${output_message}accepted")
    else()
        set(${var} FALSE PARENT_SCOPE)
        message("${output_message}not accepted")
    endif()
endfunction()

function(check_cxx_flag flag var)
    include(CheckCXXCompilerFlag)
    set(output_message "-- C++ compiler flag ${flag} - ")
    check_cxx_compiler_flag("${flag}" ${var})
    if(${${var}})
        if(2 LESS ${ARGC})
            if("DEBUG" STREQUAL "${ARGV2}")
                string(STRIP "${CMAKE_CXX_FLAGS_DEBUG} ${flag}" NEW_FLAGS)
                set(CMAKE_CXX_FLAGS_DEBUG "${NEW_FLAGS}" PARENT_SCOPE)
            elseif("RELEASE" STREQUAL "${ARGV2}")
                string(STRIP "${CMAKE_CXX_FLAGS_RELEASE} ${flag}" NEW_FLAGS)
                set(CMAKE_CXX_FLAGS_RELEASE "${NEW_FLAGS}" PARENT_SCOPE)
            else()
                message(SEND_ERROR "unknown build type ${ARGV2}")
            endif()
        else()
            string(STRIP "${CMAKE_CXX_FLAGS} ${flag}" NEW_FLAGS)
            set(CMAKE_CXX_FLAGS "${NEW_FLAGS}" PARENT_SCOPE)
        endif()
        set(${var} TRUE PARENT_SCOPE)
        message("${output_message}accepted")
    else()
        set(${var} FALSE PARENT_SCOPE)
        message("${output_message}not accepted")
    endif()
endfunction()

function(check_linker_flag flag)
    set(output_message "-- Linker flag ${flag} - ")
    if(1 LESS ${ARGC})
        if("DEBUG" STREQUAL "${ARGV1}")
            string(STRIP "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${flag}" NEW_FLAGS)
            set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${NEW_FLAGS}" PARENT_SCOPE)
        elseif("RELEASE" STREQUAL "${ARGV1}")
            string(STRIP "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${flag}" NEW_FLAGS)
            set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${NEW_FLAGS}" PARENT_SCOPE)
        else()
            message(SEND_ERROR "unknown build type ${ARGV1}")
        endif()
    else()
        string(STRIP "${CMAKE_EXE_LINKER_FLAGS} ${flag}" NEW_FLAGS)
        set(CMAKE_EXE_LINKER_FLAGS "${NEW_FLAGS}" PARENT_SCOPE)
    endif()
    message("${output_message}enabled")
endfunction()

function(find_folder_msvc var match)
    get_filename_component(current_dir "${PROJECT_SOURCE_DIR}" ABSOLUTE)
    set(find_dir "")
    set(last_dir "")
    while(NOT "${last_dir}" STREQUAL "${current_dir}")
        file(GLOB subfolders RELATIVE "${current_dir}" "${current_dir}/${match}*")
        foreach(subfolder ${subfolders})
            set(find_dir "${current_dir}/${subfolder}")
        endforeach()
        file(GLOB subfolders RELATIVE "${current_dir}/env" "${current_dir}/env/${match}*")
        foreach(subfolder ${subfolders})
            set(find_dir "${current_dir}/env/${subfolder}")
        endforeach()
        if(EXISTS "${find_dir}")
            break()
        endif()
        set(last_dir "${current_dir}")
        get_filename_component(current_dir "${last_dir}/.." ABSOLUTE)
    endwhile()
    if(2 LESS ${ARGC})
        if(EXISTS "${find_dir}/${ARGV2}")
            set(${var} "${find_dir}/${ARGV2}" PARENT_SCOPE)
        else()
            set(${var} "" PARENT_SCOPE)
        endif()
    else()
        set(${var} "${find_dir}" PARENT_SCOPE)
    endif()
endfunction()

function(check_include_directory dir)
    set(dir_mode "")
    if(1 LESS ${ARGC})
        if("SYSTEM" STREQUAL "${ARGV1}")
            set(dir_mode "system ")
        else()
            message(SEND_ERROR "unknown include directory mode ${ARGV1}")
        endif()
    endif()
    set(output_message "-- Looking for ${dir_mode}include directory '${dir}' - ")
    if(EXISTS "${dir}/")
        message("${output_message}found")
        if(NOT "" STREQUAL "${dir_mode}")
            include_directories(${ARGV1} ${dir})
        else()
            include_directories(${dir})
        endif()
    else()
        message("${output_message}not found")
    endif()
endfunction()

function(check_include_directory_msvc dir)
    set(output_message "-- Looking for include directory '${dir}' - ")
    if(1 LESS ${ARGC})
        find_folder_msvc(found_folder "${dir}" "${ARGV1}")
    else()
        find_folder_msvc(found_folder "${dir}")
    endif()
    if(NOT "" STREQUAL "${found_folder}")
        include_directories(${found_folder}/)
        message("${output_message}${found_folder}")
    else()
        message("${output_message}not found")
    endif()
endfunction()

function(check_link_directory dir)
    set(output_message "-- Looking for link directory '${dir}' - ")
    if(EXISTS "${dir}/")
        message("${output_message}found")
        link_directories(${dir})
    else()
        message("${output_message}not found")
    endif()
endfunction()

function(check_link_directory_msvc dir)
    set(output_message "-- Looking for link directory '${dir}' - ")
    if(1 LESS ${ARGC})
        find_folder_msvc(found_folder "${dir}" "${ARGV1}")
    else()
        find_folder_msvc(found_folder "${dir}")
    endif()
    if(NOT "" STREQUAL "${found_folder}")
        link_directories(${found_folder}/)
        message("${output_message}${found_folder}")
    else()
        message("${output_message}not found")
    endif()
endfunction()

function(check_mali)
    set(output_message "-- Checking for Mali - ")
    if(EXISTS "/usr/lib/arm-linux-gnueabihf/mali-egl/")
        SET(MALI_FOUND TRUE PARENT_SCOPE)
        message("${output_message}found")
    else()
        message("${output_message}not found")
    endif()
endfunction()

function(check_videocore)
    include(CheckCXXSourceCompiles)
    set(output_message "-- Checking for VideoCore - ")
    get_directory_property(include_dirs INCLUDE_DIRECTORIES)
    set(CMAKE_REQUIRED_INCLUDES ${include_dirs})
    CHECK_CXX_SOURCE_COMPILES("#include \"bcm_host.h\"\nint main(int argc, char **argv)\n{\n  (void)argc;\n  (void)argv;\n#if !defined(CEC_VERSION)\n#error \"no CEC_VERSION\"\n#endif\n  return 0;\n}" HAVE_BCM_HOST_H)
    if(HAVE_BCM_HOST_H)
        set(VIDEOCORE_FOUND TRUE PARENT_SCOPE)
        message("${output_message}found")
    else()
        message("${output_message}not found")
    endif()
endfunction()

set(MINIMUM_BOOST_VERSION 1.42.0)

function(find_boost)
    if(MSVC)
        check_include_directory_msvc("boost_")
        check_link_directory_msvc("boost_" "stage/lib")
    else()
        set(Boost_USE_STATIC_LIBS ON)
        foreach(ii ${ARGN})
            message("-- Checking for Boost ${ii}")
            find_package(Boost 1.42.0 REQUIRED ${ii})
            if(NOT Boost_FOUND)
                message(SEND_ERROR "Boost_${ii} not found")
            endif()
            STRING(TOUPPER "${ii}" UU)
            if("${ii}" MATCHES "thread")
                if("${CMAKE_SYSTEM_NAME}" MATCHES "FreeBSD")
                    LIST(APPEND Boost_LIBRARIES "thr")
                endif()
            endif()
            set("BOOST_${UU}_LIBRARY" ${Boost_LIBRARIES} PARENT_SCOPE)
            include_directories(${Boost_INCLUDE_DIRS})
        endforeach()
    endif()
endfunction()

function(find_freetype)
    if(MSVC)
        check_include_directory_msvc("freetype-" "include")
        check_link_directory_msvc("freetype-" "objs/Win32")
        set(FREETYPE_LIBRARY_DEBUG "Debug Static/freetype" PARENT_SCOPE)
        set(FREETYPE_LIBRARY "Release Static/freetype" PARENT_SCOPE)
    else()
        include(FindFreetype)
        if(FREETYPE_FOUND)
            include_directories(${FREETYPE_INCLUDE_DIRS})
            set(FREETYPE_LIBRARY ${FREETYPE_LIBRARIES} PARENT_SCOPE)
        else()
            message(SEND_ERROR "FreeType not found")
        endif()
    endif()
endfunction()

function(find_glew)
    if(MSVC)
        check_include_directory_msvc("glew-" "include")
        check_link_directory_msvc("glew-" "lib/Debug/Win32")
        check_link_directory_msvc("glew-" "lib/Release/Win32")
        add_definitions("/DGLEW_STATIC")
        set(GLEW_LIBRARY_DEBUG "glew32sd.lib" PARENT_SCOPE)
        set(GLEW_LIBRARY "glew32s.lib" PARENT_SCOPE)
    else()
        include(FindPkgConfig)
        pkg_search_module(GLEW glew)
        if(GLEW_FOUND)
            if(GLEW_INCLUDE_DIRS)
                include_directories(${GLEW_INCLUDE_DIRS})
            endif()
            if(GLEW_LIBRARY_DIRS)
                link_directories(${GLEW_LIBRARY_DIRS})
            endif()
            if(GLEW_CFLAGS)
                add_definitions(${GLEW_CFLAGS})
            endif()
            if(GLEW_CFLAGS_OTHER)
                add_definitions(${GLEW_CFLAGS_OTHER})
            endif()
            set(GLEW_LIBRARY ${GLEW_LIBRARIES} PARENT_SCOPE)
            message("-- Found GLEW: ${GLEW_VERSION}")
        else()
            message(SEND_ERROR "GLEW not found")
        endif()
    endif()
endfunction()

function(find_glu)
    include(FindOpenGL)
    if(OPENGL_GLU_FOUND)
        message("-- Found GLU: ${OPENGL_glu_LIBRARY}")
    else()
        message(SEND_ERROR "GLU required but not found")
    endif()
endfunction()

function(find_jpeg)
    if(MSVC)
        check_include_directory_msvc("jpeg-")
        check_link_directory_msvc("jpeg-" "Release")
        set(JPEG_LIBRARY_DEBUG "jpeg.lib" PARENT_SCOPE)
        set(JPEG_LIBRARY "jpeg.lib" PARENT_SCOPE)
    else()
        include(FindJPEG)
        if(JPEG_FOUND)
            include_directories(${JPEG_INCLUDE_DIR})
            set(JPEG_LIBRARY ${JPEG_LIBRARIES} PARENT_SCOPE)
        else()
            message(SEND_ERROR "JPEG not found")
        endif()
    endif()
endfunction()

function(find_ncurses)
    if(MSVC)
        check_include_directory_msvc("ncurses")
        check_link_directory_msvc("ncurses" "Release")
        set(NCURSES_LIBRARY_DEBUG "ncursesd.lib" PARENT_SCOPE)
        set(NCURSES_LIBRARY "ncurses.lib" PARENT_SCOPE)
    else()
        set(CURSES_NEED_NCURSES TRUE)
        include(FindCurses)
        if(CURSES_FOUND)
            include_directories(${CURSES_INCLUDE_DIRS})
            set(NCURSES_LIBRARY ${CURSES_LIBRARIES} PARENT_SCOPE)
        else()
            message(SEND_ERROR "ncurses not found")
        endif()
    endif()
endfunction()

function(find_ogg)
    if(MSVC)
        check_include_directory_msvc("libogg-" "include")
        check_link_directory_msvc("libogg-" "win32/VS2010/Win32")
        set(OGG_LIBRARY_DEBUG "Debug/libogg_static" PARENT_SCOPE)
        set(OGG_LIBRARY "Release/libogg_static" PARENT_SCOPE)
    else()
        include(FindPkgConfig)
        pkg_search_module(OGG ogg)
        if(OGG_FOUND)
            if(OGG_INCLUDE_DIRS)
                include_directories(${OGG_INCLUDE_DIRS})
            endif()
            if(OGG_LIBRARY_DIRS)
                link_directories(${OGG_LIBRARY_DIRS})
            endif()
            if(OGG_CFLAGS)
                add_definitions(${OGG_CFLAGS})
            endif()
            if(OGG_CFLAGS_OTHER)
                add_definitions(${OGG_CFLAGS_OTHER})
            endif()
            set(OGG_LIBRARY ${OGG_LIBRARIES} PARENT_SCOPE)
            message("-- Found Ogg: ${OGG_VERSION}")
        else()
            message(SEND_ERROR "Ogg not found")
        endif()
    endif()
endfunction()

function(find_openal)
    if(MSVC)
        check_include_directory_msvc("openal-soft-" "include")
        check_link_directory_msvc("openal-soft-")
        set(OPENAL_LIBRARY_DEBUG "Debug/OpenAL32" PARENT_SCOPE)
        set(OPENAL_LIBRARY "Release/OpenAL32" PARENT_SCOPE)
    else()
        include(FindOpenAL)
        if(OPENAL_FOUND)
            include_directories(${OPENAL_INCLUDE_DIR})
            set(OPENAL_LIBRARY ${OPENAL_LIBRARY} PARENT_SCOPE)
        else()
            message(SEND_ERROR "OpenAL not found")
        endif()
    endif()
endfunction()

function(find_opengl)
    set(OpenGL_GL_PREFERENCE "GLVND")
    include(FindOpenGL)
    if(NOT OPENGL_FOUND)
        message(SEND_ERROR "OpenGL required but not found")
    endif()
endfunction()

function(find_png)
    if(MSVC)
        check_include_directory_msvc("libpng-")
        check_include_directory_msvc("zlib-")
        check_link_directory_msvc("libpng-" "projects/vstudio")
        set(PNG_LIBRARY_DEBUG "Debug Library/libpng16" PARENT_SCOPE)
        set(PNG_LIBRARY "Release Library/libpng16" PARENT_SCOPE)
        set(ZLIB_LIBRARY_DEBUG "Debug Library/zlib" PARENT_SCOPE)
        set(ZLIB_LIBRARY "Release Library/zlib" PARENT_SCOPE)
    else()
        include(FindPNG)
        if(PNG_FOUND)
            include_directories(${PNG_INCLUDE_DIRS})
            add_definitions(${PNG_DEFINITIONS})
            set(PNG_LIBRARY ${PNG_LIBRARIES} PARENT_SCOPE)
        else()
            message(SEND_ERROR "PNG not found")
        endif()
    endif()
endfunction()

function(find_sdl)
    if(MSVC)
        check_include_directory_msvc("SDL-" "include")
        check_link_directory_msvc("SDL-" "lib/x86")
        set(SDL_LIBRARY_DEBUG "SDL" "SDLmain" PARENT_SCOPE)
        set(SDL_LIBRARY "SDL" "SDLmain" PARENT_SCOPE)
    else()
        include(FindSDL)
        if(SDL_FOUND)
            include_directories(${SDL_INCLUDE_DIR})
            set(SDL_LIBRARY "${SDL_LIBRARY}" PARENT_SCOPE)
        else()
            message(SEND_ERROR "SDL not found")
        endif()
    endif()
endfunction()

function(find_sdl2)
    if(MSVC)
        check_include_directory_msvc("SDL2-" "include")
        check_link_directory_msvc("SDL2-" "VisualC/Win32")
        set(SDL2_LIBRARY_DEBUG "Debug/SDL2" "Debug/SDL2main" PARENT_SCOPE)
        set(SDL2_LIBRARY "Release/SDL2" "Release/SDL2main" PARENT_SCOPE)
    else()
        include(FindPkgConfig)
        pkg_search_module(SDL2 sdl2)
        if(SDL2_FOUND)
            if(SDL2_INCLUDE_DIRS)
                include_directories(SYSTEM ${SDL2_INCLUDE_DIRS})
            endif()
            if(SDL2_LIBRARY_DIRS)
                link_directories(${SDL2_LIBRARY_DIRS})
            endif()
            if(SDL2_CFLAGS)
                add_definitions(${SDL2_CFLAGS})
            endif()
            if(SDL2_CFLAGS_OTHER)
                add_definitions(${SDL2_CFLAGS_OTHER})
            endif()
            set(SDL2_LIBRARY ${SDL2_LIBRARIES} PARENT_SCOPE)
            message("-- Found SDL2: ${SDL2_VERSION}")
        else()
            message(SEND_ERROR "SDL2 not found")
        endif()
    endif()
endfunction()

function(find_sndfile)
    if(MSVC)
        check_include_directory_msvc("sndfile-" "include")
        check_link_directory_msvc("sndfile-" "VisualC/Win32")
        set(SNDFILE_LIBRARY_DEBUG "Debug/sndfile" PARENT_SCOPE)
        set(SNDFILE_LIBRARY "Release/sndfile" PARENT_SCOPE)
    else()
        include(FindPkgConfig)
        pkg_search_module(SNDFILE sndfile)
        if(SNDFILE_FOUND)
            if(SNDFILE_INCLUDE_DIRS)
                include_directories(SYSTEM ${SNDFILE_INCLUDE_DIRS})
            endif()
            if(SNDFILE_LIBRARY_DIRS)
                link_directories(${SNDFILE_LIBRARY_DIRS})
            endif()
            if(SNDFILE_CFLAGS)
                add_definitions(${SNDFILE_CFLAGS})
            endif()
            if(SNDFILE_CFLAGS_OTHER)
                add_definitions(${SNDFILE_CFLAGS_OTHER})
            endif()
            set(SNDFILE_LIBRARY ${SNDFILE_LIBRARIES} PARENT_SCOPE)
            message("-- Found libsndfile: ${SNDFILE_VERSION}")
        else()
            message(SEND_ERROR "libsndfile not found")
        endif()
    endif()

endfunction()

function(find_threads)
    find_package(Threads REQUIRED)
    set(THREADS_LIBRARY Threads::Threads PARENT_SCOPE)
endfunction()

function(find_vorbis)
    if(MSVC)
        check_include_directory_msvc("libvorbis-" "include")
        check_link_directory_msvc("libvorbis-" "win32/VS2010/Win32")
        set(VORBIS_LIBRARY_DEBUG "Debug/libvorbis_static" PARENT_SCOPE)
        set(VORBIS_LIBRARY "Release/libvorbis_static" PARENT_SCOPE)
    else()
        include(FindPkgConfig)
        pkg_search_module(VORBIS vorbis)
        if(VORBIS_FOUND)
            if(VORBIS_INCLUDE_DIRS)
                include_directories(${VORBIS_INCLUDE_DIRS})
            endif()
            if(VORBIS_LIBRARY_DIRS)
                link_directories(${VORBIS_LIBRARY_DIRS})
            endif()
            if(VORBIS_CFLAGS)
                add_definitions(${VORBIS_CFLAGS})
            endif()
            if(VORBIS_CFLAGS_OTHER)
                add_definitions(${VORBIS_CFLAGS_OTHER})
            endif()
            set(VORBIS_LIBRARY ${VORBIS_LIBRARIES} PARENT_SCOPE)
            message("-- Found Vorbis: ${VORBIS_VERSION}")
        else()
            message(SEND_ERROR "Vorbis not found")
        endif()
    endif()
endfunction()

function(output_flags)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
        set(CMAKE_BUILD_TYPE "Release" PARENT_SCOPE)
    endif()
    if(ARGV0)
        if(MSVC)
            string(STRIP "${CMAKE_C_FLAGS_DEBUG} /D${ARGV0}" CMAKE_C_FLAGS_DEBUG_NEW)
            string(STRIP "${CMAKE_CXX_FLAGS_DEBUG} /D${ARGV0}" CMAKE_CXX_FLAGS_DEBUG_NEW)
        else()
            string(STRIP "${CMAKE_C_FLAGS_DEBUG} -D${ARGV0}" CMAKE_C_FLAGS_DEBUG_NEW)
            string(STRIP "${CMAKE_CXX_FLAGS_DEBUG} -D${ARGV0}" CMAKE_CXX_FLAGS_DEBUG_NEW)
        endif()
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_NEW}")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_NEW}" PARENT_SCOPE)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_NEW}")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_NEW}" PARENT_SCOPE)
    endif()
    if(1 LESS ${ARGC})
        set(CMAKE_VERBOSE_MAKEFILE ${ARGV1})
        set(CMAKE_VERBOSE_MAKEFILE ${ARGV1} PARENT_SCOPE)
        message("-- Using verbose makefiles: ${CMAKE_VERBOSE_MAKEFILE}")
    endif()
    get_directory_property(include_dirs INCLUDE_DIRECTORIES)
    get_directory_property(link_dirs LINK_DIRECTORIES)
    message("-- Using configuration: ${CMAKE_BUILD_TYPE} from ${CMAKE_CONFIGURATION_TYPES}")
    message("-- Using C compiler: ${CMAKE_C_COMPILER}")
    message("-- Using C++ compiler: ${CMAKE_CXX_COMPILER}")
    message("-- Using C compiler flags: ${CMAKE_C_FLAGS}")
    message("-- Using C++ compiler flags: ${CMAKE_CXX_FLAGS}")
    message("-- Using DEBUG C compiler flags: ${CMAKE_C_FLAGS_DEBUG}")
    message("-- Using DEBUG C++ compiler flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("-- Using RELEASE C compiler flags: ${CMAKE_C_FLAGS_RELEASE}")
    message("-- Using RELEASE C++ compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")
    message("-- Using linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
    message("-- Using DEBUG linker flags: ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
    message("-- Using RELEASE linker flags: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
    message("-- Using include directories: ${include_dirs}")
    message("-- Using link directories: ${link_dirs}")
endfunction()

function(output_flags_remove_cxx11)
    string(REPLACE " -std=c++11" "" CMAKE_CXX_FLAGS_TEMP1 ${CMAKE_CXX_FLAGS})
    string(REPLACE "-std=c++11 " "" CMAKE_CXX_FLAGS_TEMP2 ${CMAKE_CXX_FLAGS_TEMP1})
    string(REPLACE "-std=c++11" "" CMAKE_CXX_FLAGS_TEMP3 ${CMAKE_CXX_FLAGS_TEMP2})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_TEMP3} PARENT_SCOPE)
endfunction()

if(${MSVC})
    add_definitions("/D_CRT_SECURE_NO_WARNINGS")
    add_definitions("/D_SCL_SECURE_NO_WARNINGS")
    add_definitions("/DWIN32")

    #check_c_flag("/W3" HAS_C_FLAG_W3)
    #check_cxx_flag("/W3" HAS_CXX_FLAG_W3)

    #check_cxx_flag("/EHsc" HAS_CXX_FLAG_EHSC)

    #check_c_flag("/Od" HAS_C_FLAG_OD "DEBUG")
    #check_cxx_flag("/Od" HAS_CXX_FLAG_OD "DEBUG")

    #check_c_flag("/Ox" HAS_C_FLAG_OX "RELEASE")
    #check_cxx_flag("/Ox" HAS_CXX_FLAG_OX "RELEASE")
    check_c_flag("/Oi" HAS_C_FLAG_OI "RELEASE")
    check_cxx_flag("/Oi" HAS_CXX_FLAG_OI "RELEASE")
    check_c_flag("/Ot" HAS_C_FLAG_OT "RELEASE")
    check_cxx_flag("/Ot" HAS_CXX_FLAG_OT "RELEASE")
    check_c_flag("/Oy" HAS_C_FLAG_OY "RELEASE")
    check_cxx_flag("/Oy" HAS_CXX_FLAG_OY "RELEASE")
    check_c_flag("/GL" HAS_C_FLAG_GL "RELEASE")
    check_cxx_flag("/GL" HAS_CXX_FLAG_GL "RELEASE")

    #check_linker_flag("/INCREMENTAL:NO" "DEBUG")

    check_linker_flag("/LTCG" "RELEASE")
else()
    check_include_directory("/opt/include")
    check_include_directory("/opt/local/include")
    check_include_directory("/opt/vc/include" SYSTEM)
    check_include_directory("/opt/vc/include/interface/vcos/pthreads" SYSTEM)
    check_include_directory("/opt/vc/include/interface/vmcs_host/linux" SYSTEM)
    check_include_directory("/sw/include")
    check_include_directory("/usr/include")
    check_include_directory("/usr/local/include" SYSTEM)
    check_include_directory("/usr/X11R6/include")
    check_link_directory("/opt/lib")
    check_link_directory("/opt/local/lib")
    check_link_directory("/opt/vc/lib")
    check_link_directory("/sw/lib")
    check_link_directory("/usr/lib/arm-linux-gnueabihf")
    check_link_directory("/usr/local/lib")
    check_link_directory("/usr/X11R6/lib")

    check_c_flag("-Werror=unused-command-line-argument" HAS_C_FLAG_WERROR_UNUSED_COMMAND_LINE_ARGUMENT)
    check_cxx_flag("-Werror=unused-command-line-argument" HAS_CXX_FLAG_WERROR_UNUSED_COMMAND_LINE_ARGUMENT)
    check_c_flag("-Werror=unknown-warning-option" HAS_C_FLAG_WERROR_UNKNOWN_WARNING_OPTION)
    check_cxx_flag("-Werror=unknown-warning-option" HAS_CXX_FLAG_WERROR_UNKNOWN_WARNING_OPTION)

    check_c_flag("-std=c99" HAS_C_FLAG_STD_C99)
    check_cxx_flag("-std=c++17" HAS_CXX_FLAG_STD_CXX17)
    if(NOT HAS_CXX_FLAG_STD_CXX17)
        check_cxx_flag("-std=c++14" HAS_CXX_FLAG_STD_CXX14)
        if(NOT HAS_CXX_FLAG_STD_CXX14)
            check_cxx_flag("-std=c++11" HAS_CXX_FLAG_STD_CXX11)
        endif()
    endif()

    check_c_flag("-Werror=return-type" HAS_C_FLAG_WERROR_RETURN_TYPE)
    check_cxx_flag("-Werror=return-type" HAS_CXX_FLAG_WERROR_RETURN_TYPE)

    check_c_flag("-Werror-implicit-function-declaration" HAS_C_FLAG_WERROR_IMPLICIT_FUNCTION_DECLARATION)

    check_cxx_flag("-Werror=non-virtual-dtor" HAS_CXX_FLAG_WERROR_NON_VIRTUAL_DTOR)

    check_c_flag("-Wall" HAS_C_FLAG_WALL)
    check_cxx_flag("-Wall" HAS_CXX_FLAG_WALL)
    check_c_flag("-Wcast-align" HAS_C_FLAG_WCAST_ALIGN)
    check_cxx_flag("-Wcast-align" HAS_CXX_FLAG_WCAST_ALIGN)
    check_c_flag("-Wconversion" HAS_C_FLAG_WCONVERSION)
    check_cxx_flag("-Wconversion" HAS_CXX_FLAG_WCONVERSION)
    check_c_flag("-Wextra" HAS_C_FLAG_WEXTRA)
    check_cxx_flag("-Wextra" HAS_CXX_FLAG_WEXTRA)
    check_c_flag("-Winit-self" HAS_C_FLAG_WINIT_SELF)
    check_cxx_flag("-Winit-self" HAS_CXX_FLAG_WINIT_SELF)
    check_c_flag("-Winvalid-pch" HAS_C_FLAG_WINVALID_PCH)
    check_cxx_flag("-Winvalid-pch" HAS_CXX_FLAG_WINVALID_PCH)
    check_c_flag("-Wlogical-op" HAS_C_FLAG_WLOGICAL_OP)
    check_cxx_flag("-Wlogical-op" HAS_CXX_FLAG_WLOGICAL_OP)
    check_c_flag("-Wmissing-format-attribute" HAS_C_FLAG_WMISSING_FORMAT_ATTRIBUTE)
    check_cxx_flag("-Wmissing-format-attribute" HAS_CXX_FLAG_WMISSING_FORMAT_ATTRIBUTE)
    check_c_flag("-Wmissing-include-dirs" HAS_C_FLAG_WMISSING_INCLUDE_DIRS)
    check_cxx_flag("-Wmissing-include-dirs" HAS_CXX_FLAG_WMISSING_INCLUDE_DIRS)
    check_c_flag("-Wpacked" HAS_C_FLAG_WPACKED)
    check_cxx_flag("-Wpacked" HAS_CXX_FLAG_WPACKED)
    check_c_flag("-Wredundant-decls" HAS_C_FLAG_WREDUNDANT_DECLS)
    check_cxx_flag("-Wredundant-decls" HAS_CXX_FLAG_WREDUNDANT_DECLS)
    check_c_flag("-Wshadow" HAS_C_FLAG_WSHADOW)
    check_cxx_flag("-Wshadow" HAS_CXX_FLAG_WSHADOW)
    check_c_flag("-Wswitch-default" HAS_C_FLAG_WSWITCH_DEFAULT)
    check_cxx_flag("-Wswitch-default" HAS_CXX_FLAG_WSWITCH_DEFAULT)
    #check_c_flag("-Wswitch-enum" HAS_C_FLAG_SWITCH_ENUM)
    #check_cxx_flag("-Wswitch-enum" HAS_CXX_FLAG_SWITCH_ENUM)
    check_c_flag("-Wwrite-strings" HAS_C_FLAG_WWRITE_STRINGS)
    check_cxx_flag("-Wwrite-strings" HAS_CXX_FLAG_WWRITE_STRINGS)
    check_c_flag("-Wundef" HAS_C_FLAG_WUNDEF)
    check_cxx_flag("-Wundef" HAS_CXX_FLAG_WUNDEF)

    check_c_flag("-Wbad-function-cast" HAS_C_FLAG_WBAD_FUNCTION_CAST)
    check_c_flag("-Wmissing-declarations" HAS_C_FLAG_WMISSING_DECLARATIONS)
    check_c_flag("-Wmissing-prototypes" HAS_C_FLAG_WMISSING_PROTOTYPES)
    check_c_flag("-Wnested-externs" HAS_C_FLAG_WNESTED_EXTERNS)
    check_c_flag("-Wold-style-definition" HAS_C_FLAG_WOLD_STYLE_DEFINITION)
    check_c_flag("-Wstrict-prototypes" HAS_C_FLAG_WSTRICT_PROTOTYPES)

    check_cxx_flag("-Wctor-dtor-privacy" HAS_CXX_FLAG_WCTOR_DTOR_PRIVACY)
    check_cxx_flag("-Wold-style-cast" HAS_CXX_FLAG_WOLD_STYLE_CAST)
    check_cxx_flag("-Woverloaded-virtual" HAS_CXX_FLAG_WOVERLOADED_VIRTUAL)

    check_c_flag("-fdiagnostics-color=auto" HAS_C_FLAG_FDIAGNOSTICS_COLOR)
    check_cxx_flag("-fdiagnostics-color=auto" HAS_CXX_FLAG_FDIAGNOSTICS_COLOR)
    check_c_flag("-fdiagnostics-show-option" HAS_C_FLAG_FDIAGNOSTICS_SHOW_OPTION)
    check_cxx_flag("-fdiagnostics-show-option" HAS_CXX_FLAG_FDIAGNOSTICS_SHOW_OPTION)
    check_c_flag("-flto" HAS_C_FLAG_FLTO)
    check_cxx_flag("-flto" HAS_CXX_FLAG_FLTO)
    check_c_flag("-ftracer" HAS_C_FLAG_FTRACER)
    check_cxx_flag("-ftracer" HAS_CXX_FLAG_FTRACER)
    check_c_flag("-fvisibility=hidden" HAS_C_FLAG_FVISIBILITY_HIDDEN)
    check_cxx_flag("-fvisibility=hidden" HAS_CXX_FLAG_FVISIBILITY_HIDDEN)
    check_c_flag("-fweb" HAS_C_FLAG_FWEB)
    check_cxx_flag("-fweb" HAS_CXX_FLAG_FWEB)
    check_c_flag("-pipe" HAS_C_FLAG_PIPE)
    check_cxx_flag("-pipe" HAS_CXX_FLAG_PIPE)

    #check_c_flag("-g" HAS_C_FLAG_G "DEBUG")
    #check_cxx_flag("-g" HAS_CXX_FLAG_G "DEBUG")
    check_c_flag("-O0" HAS_C_FLAG_O0 "DEBUG")
    check_cxx_flag("-O0" HAS_CXX_FLAG_O0 "DEBUG")
    check_c_flag("-funit-at-a-time" HAS_C_FLAG_FUNIT_AT_A_TIME "DEBUG")
    check_cxx_flag("-funit-at-a-time" HAS_CXX_FLAG_FUNIT_AT_A_TIME "DEBUG")

    #check_c_flag("-O3" HAS_C_FLAG_O3 "RELEASE")
    #check_cxx_flag("-O3" HAS_CXX_FLAG_O3 "RELEASE")
    check_c_flag("-ffast-math" HAS_C_FLAG_FFAST_MATH "RELEASE")
    check_cxx_flag("-ffast-math" HAS_CXX_FLAG_FFAST_MATH "RELEASE")
    check_c_flag("-fgcse-las" HAS_C_FLAG_FGCSE_LAS "RELEASE")
    check_cxx_flag("-fgcse-las" HAS_CXX_FLAG_FGCSE_LAS "RELEASE")
    check_c_flag("-fgcse-sm" HAS_C_FLAG_FGCSE_SM "RELEASE")
    check_cxx_flag("-fgcse-sm" HAS_CXX_FLAG_FGCSE_SM "RELEASE")
    check_c_flag("-fomit-frame-pointer" HAS_C_FLAG_FOMIT_FRAME_POINTER "RELEASE")
    check_cxx_flag("-fomit-frame-pointer" HAS_CXX_FLAG_FOMIT_FRAME_POINTER "RELEASE")
    check_c_flag("-fsee" HAS_C_FLAG_FSEE "RELEASE")
    check_cxx_flag("-fsee" HAS_CXX_FLAG_FSEE "RELEASE")
    #check_c_flag("-fsingle-precision-constant" HAS_C_FLAG_FSINGLE_PRECISION_CONSTANT "RELEASE")
    #check_cxx_flag("-fsingle-precision-constant" HAS_CXX_FLAG_FSINGLE_PRECISION_CONSTANT "RELEASE")
    check_c_flag("-funsafe-math-optimizations" HAS_C_FLAG_FUNSAFE_MATH_OPTIMIZATIONS "RELEASE")
    check_cxx_flag("-funsafe-math-optimizations" HAS_CXX_FLAG_FUNSAFE_MATH_OPTIMIZATIONS "RELEASE")

    check_linker_flag("-s" "RELEASE")
endif()
