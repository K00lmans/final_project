cmake_minimum_required(VERSION 3.12)


# libraries are marked STATIC here because i want the server binary to be nice and self-contained; if there are ever any config files those will get placed in either ~/.config/clueserv/* or just directly thrown in home as a ~/.clueserv file

add_library(game-serverside STATIC
    # public headers
    include/game/game-in-progress.hpp
    include/game/game-startup.hpp
    include/game/player.hpp
    include/game/game-data.hpp
    
    # implementation files
    game-in-progress.cpp
    game-startup.cpp
    game-data.cpp
)
target_include_directories(game-serverside PUBLIC include)

add_library(private-game-utils STATIC
    # private headers/implementation files
    cards.hpp
    parsing.hpp
    cards.cpp
)
target_link_libraries(private-game-utils PUBLIC socket-handling)

target_link_libraries(game-serverside PRIVATE private-game-utils)
target_link_libraries(game-serverside PUBLIC socket-handling)
