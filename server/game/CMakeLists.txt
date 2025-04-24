cmake_minimum_required(VERSION 3.28)

add_library(game-serverside
    # public headers
    include/game/game-in-progress.hpp
    include/game/game-startup.hpp
    include/game/player.hpp
    include/game/game-data.hpp
    include/cards.hpp
    
    # private headers
    parsing.hpp

    # implementation files
    game-in-progress.cpp
    game-startup.cpp
)

target_compile_features(game-serverside PUBLIC cxx_std_23)
target_include_directories(game-serverside PUBLIC include)
target_link_libraries(game-serverside PUBLIC socket-handling)
