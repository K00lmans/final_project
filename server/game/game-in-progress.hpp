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
#include <algorithm>
#include <vector>
#include <socket-handling/shutdown.hpp>
#include <string>
#include <socket-handling/fd-poll.hpp>
#include "player.hpp"
#include "game-data.hpp"
#include "game-startup.hpp"

class GameInProgress {
    public:
    GameInProgress() = delete;
    GameInProgress(GameData &&game) : game(std::move(game)) {}

    ~GameInProgress() = default;

    GameInProgress(const GameInProgress &) = delete;
    GameInProgress &operator=(const GameInProgress &) = delete;

    GameInProgress(GameInProgress &&) = default;
    GameInProgress &operator=(GameInProgress &&) = default;
    GameInProgress(GameStartup &&start) : game(std::move(start.game)){}

    // returns true if you need to keep calling back, false if not
    bool callback();
    // returns a fd that has a read event whenever the callback can make progress
    int get_fd() { return game.poll.fd(); }
    private:
    // finds a file descriptor in the player list
    Player &find_io(int fd) {
        return *std::find_if(game.players.begin(), game.players.end(), [fd](const Player &player){ return fd == player.fd; });
    }
    void send_err_msg(const std::string &why) {
        std::shared_ptr<std::string> msg(new std::string("ERR," + why + "\n"));
        for (Player &player : game.players) {
            player.outbuf.add_message(msg);
        }
    }
    void send_ending_msg() {
        std::shared_ptr<std::string> msg(new std::string("GAME-END\n"));
        for (Player &player : game.players) {
            player.outbuf.add_message(msg);
        }
    }

    std::size_t turn_index = 0;
    GameData game;
};
