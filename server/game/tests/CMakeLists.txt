cmake_minimum_required(VERSION 3.12)


add_executable(test-parsing test-parsing.cpp)
target_link_libraries(test-parsing PRIVATE private-game-utils sockfuncs)


add_test(
    NAME test-parsing
    COMMAND $<TARGET_FILE:test-parsing>
)

