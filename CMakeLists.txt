cmake_minimum_version(VERSION 3.30)
project(i-havent-a-clue)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(client)
add_subdirectory(server)

add_custom_target(client client_bin)
add_custom_target(server server_bin)
