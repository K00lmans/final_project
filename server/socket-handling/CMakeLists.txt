cmake_minimum_required(VERSION 3.28)

add_subdirectory(tests)

add_library(socket-handling
    fd-utils.hpp
    fd-utils.cpp
    output-buffer.hpp
    output-buffer.cpp
    input-buffer.hpp
    fd-poll.hpp
    fd-poll.cpp
    shutdown.hpp
)

target_compile_features(socket-handling PUBLIC cxx_std_23)
