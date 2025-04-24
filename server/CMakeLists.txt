cmake_minimum_required(VERSION 3.12)


add_subdirectory(socket-handling)
add_subdirectory(game)

add_executable(echo-test echo-test.cpp)
target_link_libraries(echo-test PRIVATE socket-handling)
target_compile_features(echo-test PUBLIC cxx_std_20)

add_executable(server-bin server-bin.cpp)
target_link_libraries(server-bin PRIVATE game-serverside)
target_compile_features(server-bin PUBLIC cxx_std_20)

