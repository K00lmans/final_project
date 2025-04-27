#pragma once

#include <array>
#include <string>
#include <memory>
#include <vector>
#include <random>

#include <socket-handling/fd-poll.hpp>
#include <socket-handling/shutdown.hpp>
#include "player.hpp"


//
// Maintainer: Athena Boose
//
// Description: A struct wrapping the core data necessary for any instance of the game. This can be passed between different types of game instances and allows for bundling the state together in a nice package.
//

class GameData {
    public:
    GameData() = delete;
    GameData(std::shared_ptr<Shutdown<>> shutdown, std::mt19937 &randomizer);
    ~GameData() {
        for (Player &player : players) {
            shutdown_singleton->shutdown_sock(player.fd, std::move(player.outbuf));
        }
    }

    GameData(const GameData &) = delete;
    GameData &operator=(const GameData &) = delete;

    GameData(GameData &&) = default;
    GameData &operator=(GameData &&data);

    void add_player(int fd) {
        epoll_event ev{.events = EPOLLRDHUP | EPOLLET, .data = {.fd = fd}};
        poll.ctl(EPOLL_CTL_ADD, fd, ev);
        players.emplace_back(fd);
    }
    // this function clears everything EXCEPT the win cards and shutdown singleton
    void clear();
    int get_fd() const { return poll.fd(); }

    std::vector<Player> &get_players() { return players; }
    const std::vector<Player> &get_players() const { return players; }

    std::array<std::string, 3> &get_win_cards() { return win_cards; }
    const std::array<std::string, 3> &get_win_cards() const { return win_cards; }

    epoll_event wait_for_event(int time_ms) {
        std::array<epoll_event, 1> arr;
        poll.wait(std::span(arr), time_ms);
        return arr[0];
    }

    void mod_poll_fd(int fd, epoll_event ev) { poll.ctl(EPOLL_CTL_MOD, fd, ev); }

    private:
    std::vector<Player> players;
    std::array<std::string, 3> win_cards;
    FdPoll poll;
    std::shared_ptr<Shutdown<>> shutdown_singleton;
};
