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
#include <game/game-startup.hpp>
#include "player.hpp"
#include "game-data.hpp"


class GameInProgress {
    public:
    GameInProgress() = delete;
    GameInProgress(GameData &&game) : game(std::move(game)) {}

    ~GameInProgress() = default;

    GameInProgress(const GameInProgress &) = delete;
    GameInProgress &operator=(const GameInProgress &) = delete;

    GameInProgress(GameInProgress &&) = default;
    GameInProgress &operator=(GameInProgress &&) = default;

    GameInProgress(GameStartup &&startup) : game(std::move(startup.get_gamedata())) {}

    // returns true if you need to keep calling back, false if not
    bool callback();
    // returns a fd that has a read event whenever the callback can make progress
    int get_fd() { return game.get_fd(); }
    private:
    // finds a file descriptor in the player list
    Player &find_io(int fd) {
        return *std::find_if(game.get_players().begin(), game.get_players().end(), [fd](const Player &player){ return fd == player.fd; });
    }
    void broadcast(const std::string &message);
    bool flush_out();
    void send_err_msg(const std::string &why) {
        broadcast("ERR," + why + "\r\n");
    }
    void send_ending_msg(const std::string &why) {
        broadcast("GAME-END," + why + "\r\n");
    }
    bool other_player_msg(int fd);
    bool current_player_msg();
    std::optional<std::size_t> search_for_players() const;
    bool handle_accuse(std::size_t msg_end);

    GameData game;
    std::array<bool, 6> players_in_game{true, true, true, true, true, true};
    std::size_t turn_index = 0;
    enum {
        WaitingForCards,
        WaitingForTurnEnd,
    } turn_state;
};
