#pragma once
#include <cstddef>
#include <sys/resource.h>
#include <string>
#include <random>
#include <unordered_map>

#include <socket-handling/input-buffer.hpp>
#include <socket-handling/fd-poll.hpp>
#include <socket-handling/shutdown.hpp>
#include <socket-handling/connection-factory.hpp>
#include <game/game-startup.hpp>
#include <game/game-in-progress.hpp>

#define ever (;;) // this is funny so i did it


class GameHandler {
    public:
    GameHandler() = delete;
    GameHandler(const std::string &port_no);
    ~GameHandler() = default;

    GameHandler(const GameHandler &) = delete;
    GameHandler &operator=(const GameHandler &) = delete;

    GameHandler (GameHandler &&) = default;
    GameHandler &operator=(GameHandler &&) = default;

    void run_event();

    private:
    int get_event() {
        std::array<epoll_event, 1> ev;
        return poll.wait(std::span(ev), -1)[0].data.fd;
    }
    void handle_gamestartup(GameStartup::StartState state);
    ConnectionFactory connfact;
    std::shared_ptr<Shutdown<>> shut{new Shutdown<>{}};
    std::mt19937 randomizer{std::random_device{}()};
    GameStartup pending_game{shut, randomizer};
    std::unordered_map<int, GameInProgress> running_games;
    FdPoll poll;
};

// there's not really a good way to make this cleaner, the final event loop's just gonna be large, complicated, and annoying as hell
// breaking it up into a bunch of functions would just obscure things

template <std::size_t INPUT_BUF_SIZE>
void run_server(const std::string &port, rlim_t fd_limit) {
    rlimit lim;
    if (getrlimit(RLIMIT_NOFILE, &lim) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to get current RLIMIT_NOFILE settings.");
    }
    lim.rlim_cur = fd_limit;
    if (setrlimit(RLIMIT_NOFILE, &lim) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to set RLIMIT_NOFILE to desired value.");
    }

    GameHandler handler("12345");
    for ever {
        handler.run_event();
    }
    
}
