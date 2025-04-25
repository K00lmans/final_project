#pragma once

#include <array>
#include <string>
#include <memory>
#include <vector>

#include <socket-handling/fd-poll.hpp>
#include <socket-handling/shutdown.hpp>
#include "player.hpp"


//
// Maintainer: Athena Boose
//
// Description: A struct wrapping the core data necessary for any instance of the game. This can be passed between different types of game instances and allows for bundling the state together in a nice package.
//

struct GameData {
    GameData() = delete;
    GameData(std::shared_ptr<Shutdown<>> shutdown) : shutdown_singleton(shutdown) {}
    ~GameData() {
        for (Player &player : players) {
            shutdown_singleton->shutdown_sock(player.fd, std::move(player.outbuf));
        }
    }

    GameData(const GameData &) = delete;
    GameData &operator=(const GameData &) = delete;

    GameData(GameData &&) = default;
    GameData &operator=(GameData &&data) {
        for (Player &player : players) {
            shutdown_singleton->shutdown_sock(player.fd, std::move(player.outbuf));
        }
        players = std::move(data.players);
        poll = std::move(data.poll);
        win_cards = std::move(data.win_cards);
        shutdown_singleton = std::move(data.shutdown_singleton);
        return *this;
    }

    void add_player(int fd) {
        epoll_event ev{.events = EPOLLRDHUP, .data = {.fd = fd}};
        poll.ctl(EPOLL_CTL_ADD, fd, ev);
        players.emplace_back(fd);
    }

    std::vector<Player> players;
    FdPoll poll;
    std::array<std::string, 3> win_cards;
    std::shared_ptr<Shutdown<>> shutdown_singleton;
};
