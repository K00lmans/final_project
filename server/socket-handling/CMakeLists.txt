cmake_minimum_required(VERSION 3.12)

add_subdirectory(tests)

add_library(socket-handling STATIC
    include/socket-handling/fd-utils.hpp
    include/socket-handling/output-buffer.hpp
    include/socket-handling/input-buffer.hpp
    include/socket-handling/fd-poll.hpp
    include/socket-handling/shutdown.hpp
    include/socket-handling/timer.hpp
    fd-utils.cpp
    output-buffer.cpp
    fd-poll.cpp
    timer.cpp
    connection-factory.cpp
)

target_compile_features(socket-handling PUBLIC cxx_std_20)
target_include_directories(socket-handling PUBLIC include)
