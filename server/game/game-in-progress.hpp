#pragma once

//
// Maintainer: Athena Boose
//
// Description: A class to handle game logic for an in-progress game.
//
// Tags:
//
// NOT_IMPLEMENTED
// NO_TESTS
//

#include <memory>
#include <vector>
#include <socket-handling/shutdown.hpp>
#include "player.hpp"

class GameInProgress {
    public:
    GameInProgress() = delete;
    GameInProgress(
        std::vector<Player> &&players, 
        std::array<std::string, 3> win_cards, 
        std::shared_ptr<Shutdown<>> shutdown_class
    );

    GameInProgress(const GameInProgress &) = delete;
    GameInProgress &operator=(const GameInProgress &) = delete;

    GameInProgress(GameInProgress &&) = default;
    GameInProgress &operator=(GameInProgress &&) = default;

    // returns true if you need to keep calling back, false if not
    bool callback();
    // returns a fd that has a read event whenever the callback can make progress
    int get_fd();
    private:
    Player &find_io(int fd);
    std::vector<Player> players;
    std::size_t turn_index;
    std::array<std::string, 3> win_cards;
};
