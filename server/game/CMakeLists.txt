cmake_minimum_required(VERSION 3.28)

add_library(game-serverside
    game-in-progress.hpp
    game-in-progress.cpp
    player.hpp
)

target_compile_features(game-serverside PUBLIC cxx_std_23)
target_link_libraries(game-serverside PUBLIC socket-handling)
