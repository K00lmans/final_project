cmake_minimum_required(VERSION 3.30)
project(i-havent-a-clue CXX)

set(CMAKE_CXX_STANDARD 20)
add_custom_target(client)
add_custom_target(server)

add_subdirectory(client)
add_subdirectory(server)

add_dependencies(client client-bin)
add_dependencies(server server-bin)

