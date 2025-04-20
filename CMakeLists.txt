cmake_minimum_required(VERSION 3.28)
project(i-havent-a-clue CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug) # change to Release when building a release!!
include(CTest)

if (NOT DEFINED BUILD_CLIENT)
    message(NOTICE "Option `BUILD_CLIENT` not defined, assuming you want to build client.")
    add_subdirectory(client)
elseif(BUILD_CLIENT)
    message(NOTICE "Option `BUILD_CLIENT` enabled, building client.")
    add_subdirectory(client)
else()
    message(NOTICE "Option `BUILD_CLIENT` disabled, not building client.")
endif()

# server code won't work on windows so you have to pass a special flag to actually make it build
if (DEFINED BUILD_SERVER)
    if (BUILD_SERVER)
        message(NOTICE "Option `BUILD_SERVER` enabled, building server.")
        add_subdirectory(server)
    else()
        message(NOTICE "Option `BUILD_SERVER` disabled, not building server.")
    endif()
else()
    message(NOTICE "Option `BUILD_SERVER` not defined, assuming you don't want to build server.")
endif()

