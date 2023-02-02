cmake_minimum_required(VERSION 3.17)

project(Luna
        VERSION 1.0.0
        DESCRIPTION "HL1 Lua Plugin Manager"
        HOMEPAGE_URL "https://github.com/Amaroq7/Luna"
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)

if (WIN32)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message(FATAL_ERROR "Only MSVC is supported.")
    endif ()

    include(InstallRequiredSystemLibraries)

    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
    endif ()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else ()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(IS_CLANG_COMPILER ON)

        string(FIND ${CMAKE_CXX_COMPILER_VERSION} "." FIRST_DOT)
        string(SUBSTRING ${CMAKE_CXX_COMPILER_VERSION} 0 ${FIRST_DOT} CLANG_MAJOR_VER)
        find_program(LLD NAMES lld-${CLANG_MAJOR_VER} lld REQUIRED)
        get_filename_component(LLD ${LLD} NAME)
    endif ()
endif ()

include(cmake/BuildFMT.cmake)
include(cmake/BuildYAML.cmake)
include(cmake/BuildLua.cmake)

set(DEPS_INCLUDE_DIR deps)

add_subdirectory(luna)
