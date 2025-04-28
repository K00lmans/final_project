cmake_minimum_required(VERSION 3.12)
project(i-havent-a-clue CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CTest)

if(DEFINED USE_ASAN)
    if (USE_ASAN)

    message(NOTICE "Option `USE_ASAN` specified. Adding extra compiler warnings and linking ASAN.")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize-recover=address -fsanitize=undefined -fsanitize=leak") # comment this out if you want to check for leaks with valgrind
    else()
    message(NOTICE "Option `USE_ASAN` disabled, not linking ASAN.")
    endif()
else()
message(NOTICE "Option `USE_ASAN` not defined, not linking ASAN.")
endif()


# if i'm running this on some headless Linux server somewhere I don't want to have to install SFML's dependencies
if (NOT DEFINED BUILD_CLIENT)
    message(NOTICE "Option `BUILD_CLIENT` not defined, defaulting to building client.")
    add_subdirectory(client)
elseif(BUILD_CLIENT)
    message(NOTICE "Option `BUILD_CLIENT` enabled, building client.")
    add_subdirectory(client)
else()
    message(NOTICE "Option `BUILD_CLIENT` disabled, not building client.")
endif()

# server code won't work on windows so you have to pass a special flag to actually make it build
# people use IDEs and this makes it easy for them ;-;
if (DEFINED BUILD_SERVER)
    if (BUILD_SERVER)
        message(NOTICE "Option `BUILD_SERVER` enabled, building server.")
        if (NOT DEFINED PORT)
            set(PORT "\"12345\"")
        endif()
        add_subdirectory(server)
    else()
        message(NOTICE "Option `BUILD_SERVER` disabled, not building server.")
    endif()
else()
    message(NOTICE "Option `BUILD_SERVER` not defined, defaulting to not building server.")
endif()

