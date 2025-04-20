cmake_minimum_required(VERSION 3.28)
project(i-havent-a-clue CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug) # change to Release when building a release!!
include(CTest)

add_subdirectory(client)

# server code won't work on windows so you have to pass a special flag to actually make it build
if (DEFINED BUILD_SERVER)
    if (BUILD_SERVER)
        message(STATUS "Option `BUILD_SERVER` selected. Building server...")
        add_subdirectory(server)
    endif()
endif()
