cmake_minimum_required(VERSION 3.28)


add_executable(server-bin asdf.cpp)

set_target_properties(server-bin PROPERTIES
    CXX_VERSION 23
    CXX_EXTENSIONS OFF
)

add_library(socket-handling
    sock-rw.hpp
    sock-rw.cpp
    game.cpp
    game.hpp
)
