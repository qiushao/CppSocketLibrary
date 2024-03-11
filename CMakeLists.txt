cmake_minimum_required(VERSION 3.26)
project(CppSocketLibrary)

set(CMAKE_CXX_STANDARD 14)

option(ENABLE_EXAMPLE "Enable example or not" ON)
option(ENABLE_TEST "Enable test or not" OFF)

add_subdirectory(src)

if (ENABLE_EXAMPLE)
    add_subdirectory(example)
endif ()

if (ENABLE_TEST)
    add_subdirectory(test)
endif ()
