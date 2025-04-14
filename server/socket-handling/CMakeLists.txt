cmake_minimum_required(VERSION 3.28)



add_library(socket-handling
    sock-rw.hpp
    sock-rw.cpp
    message.hpp
    output-buffer.hpp
    input-buffer.hpp
)

set_target_properties(socket-handling PROPERTIES
    CXX_VERSION 23
    CXX_EXTENSIONS OFF
)
