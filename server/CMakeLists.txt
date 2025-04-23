cmake_minimum_required(VERSION 3.28)


add_subdirectory(socket-handling)
add_subdirectory(game)
add_executable(server-bin echo-test.cpp)
target_link_libraries(server-bin PUBLIC socket-handling)

set_target_properties(server-bin PROPERTIES
    CXX_VERSION 23
    CXX_EXTENSIONS OFF
)

