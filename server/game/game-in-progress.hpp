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
#include <socket-handling/fd-poll.hpp>
#include "player.hpp"

class GameInProgress {
    public:
    GameInProgress() = delete;
    GameInProgress(
        std::vector<Player> &&players, 
        std::array<std::string, 3> &&win_cards, 
        std::shared_ptr<Shutdown<>> shutdown
    ) : players(std::move(players)), 
        win_cards(std::move(win_cards)), 
        shutdown_singleton(shutdown) {}

    ~GameInProgress() {
        for (Player &player : players) {
            shutdown_singleton->shutdown(player.fd, std::move(player.outbuf));
        }
    }

    GameInProgress(const GameInProgress &) = delete;
    GameInProgress &operator=(const GameInProgress &) = delete;

    GameInProgress(GameInProgress &&) = default;
    GameInProgress &operator=(GameInProgress &&) = default;

    // returns true if you need to keep calling back, false if not
    bool callback();
    // returns a fd that has a read event whenever the callback can make progress
    int get_fd() { return poll.fd(); }
    private:
    // finds a file descriptor in the player list
    Player &find_io(int fd) {
        return *std::find_if(players.begin(), players.end(), [fd](const Player &player){ return fd == player.fd; });
    }
    void send_err_msg(const std::string &why) {
        std::shared_ptr<std::string> msg(new std::string("ERR," + why + "\n"));
        for (Player &player : players) {
            player.outbuf.add_message(msg);
        }
    }
    void send_ending_msg() {
        std::shared_ptr<std::string> msg(new std::string("GAME-END\n"));
        for (Player &player : players) {
            player.outbuf.add_message(msg);
        }
    }

    std::vector<Player> players;
    std::size_t turn_index = 0;
    std::array<std::string, 3> win_cards;
    FdPoll poll;
    std::shared_ptr<Shutdown<>> shutdown_singleton;
};
