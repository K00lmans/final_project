cmake_minimum_required(VERSION 3.25)


add_subdirectory(socket-handling)
add_subdirectory(game)

add_executable(echo-test echo-test.cpp)
target_link_libraries(echo-test PUBLIC socket-handling)
target_compile_features(echo-test PUBLIC cxx_std_20)


