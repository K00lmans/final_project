cmake_minimum_required(VERSION 3.28)
project(i-havent-a-clue CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug) # change to Release when building a release!!
include(CTest)

add_subdirectory(client)
add_subdirectory(server)

