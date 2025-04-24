cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(client-bin
        main.cpp
        Scratch_Pad.cpp
        Scratch_Pad.h
 "GameCode/Board.cpp" "GameCode/BoardSetup.cpp" "GameCode/Clue.cpp" "GameCode/Player.cpp" "GameCode/Room.cpp" "GameCode/Tile.cpp" "GameCode/Token.cpp" "GameCode/TokenID.cpp" "GameCode/BoardData.cpp"
        main.h
        functions.cpp
)

target_link_libraries(client-bin PRIVATE SFML::Graphics)
set_target_properties(client-bin PROPERTIES
    CXX_VERSION 26
    CXX_EXTENSIONS OFF
)
