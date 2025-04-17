cmake_minimum_required(VERSION 3.28)

add_library(sockfuncs sockfuncs.cpp sockfuncs.hpp)

add_executable(test-sockfuncs test-sockfuncs.cpp)
target_link_libraries(test-sockfuncs sockfuncs)

add_test(
    NAME test-sockfuncs
    COMMAND $<TARGET_FILE:test-sockfuncs>
)

