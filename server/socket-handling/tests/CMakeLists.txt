cmake_minimum_required(VERSION 3.28)

add_library(sockfuncs sockfuncs.cpp sockfuncs.hpp)

add_executable(test-sockfuncs test-sockfuncs.cpp)
target_link_libraries(test-sockfuncs sockfuncs)

add_executable(test-fd-utils test-fd-utils.cpp)
target_link_libraries(test-fd-utils sockfuncs socket-handling)

add_executable(test-input-buffer test-input-buffer.cpp)
target_link_libraries(test-input-buffer sockfuncs socket-handling)

add_test(
    NAME test-sockfuncs
    COMMAND $<TARGET_FILE:test-sockfuncs>
)

add_test(
    NAME test-fd-utils
    COMMAND $<TARGET_FILE:test-fd-utils>
)

add_test(
    NAME test-input-buffer
    COMMAND $<TARGET_FILE:test-input-buffer>
)

