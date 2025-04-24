cmake_minimum_required(VERSION 3.12)

add_library(sockfuncs sockfuncs.cpp sockfuncs.hpp)

add_executable(test-sockfuncs test-sockfuncs.cpp)
target_link_libraries(test-sockfuncs sockfuncs)
target_compile_features(test-sockfuncs PUBLIC cxx_std_20)

add_executable(test-fd-utils test-fd-utils.cpp)
target_link_libraries(test-fd-utils sockfuncs socket-handling)
target_compile_features(test-fd-utils PUBLIC cxx_std_20)

add_executable(test-input-buffer test-input-buffer.cpp)
target_link_libraries(test-input-buffer sockfuncs socket-handling)
target_compile_features(test-fd-utils PUBLIC cxx_std_20)

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

