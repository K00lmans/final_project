#pragma once
#include <cstddef>
#include <sys/resource.h>
#include <string>
#include <random>
#include <socket-handling/input-buffer.hpp>
#include <socket-handling/fd-poll.hpp>
#include <socket-handling/shutdown.hpp>
#include <socket-handling/connection-factory.hpp>
#include <game/game-startup.hpp>

#define ever (;;) // this is funny so i did it

template <std::size_t INPUT_BUF_SIZE>
void kill_sock(int sock, FdPoll &poll, Shutdown<> &shut, std::unordered_map<int, std::pair<InputBuffer<INPUT_BUF_SIZE>, OutputBuffer>> &fdmap) {
    epoll_event ev;
    poll.ctl(EPOLL_CTL_DEL, sock, ev);
    shut.shutdown_sock(sock, std::move(fdmap.at(sock).second));
    fdmap.erase(sock);
}

void renew_game(GameStartup &pending_game, std::shared_ptr<Shutdown<>> shut, std::mt19937 &randomizer, FdPoll &poll);

void handle_gamestartup(GameStartup::StartState state, GameStartup &pending_game, std::shared_ptr<Shutdown<>> shut, std::mt19937 &randomizer, FdPoll &poll);

// there's not really a good way to make this cleaner, the final event loop's just gonna be large, complicated, and annoying as hell
// breaking it up into a bunch of functions would just obscure things

template <std::size_t INPUT_BUF_SIZE>
void run_server(const std::string &port, rlim_t fd_limit) {
    // server setup
    std::mt19937 randomizer{std::random_device{}()};

    FdPoll poll;
    std::array<epoll_event, 1> ep_array;
    std::span span(ep_array);

    ConnectionFactory connfact("12345");
    epoll_event ev{.events = EPOLLIN | EPOLLERR, .data = {.fd = connfact.get_fd()}};
    poll.ctl(EPOLL_CTL_ADD, connfact.get_fd(), ev);

    std::shared_ptr<Shutdown<>> shut(new Shutdown<>());
    ev.data.fd = shut->get_fd();
    poll.ctl(EPOLL_CTL_ADD, shut->get_fd(), ev);

    GameStartup pending_game{shut, randomizer};
    ev.data.fd = pending_game.get_fd();
    poll.ctl(EPOLL_CTL_ADD, pending_game.get_fd(), ev);

    ev.data.fd = STDIN_FILENO;
    poll.ctl(EPOLL_CTL_ADD, STDIN_FILENO, ev);

    rlimit lim;
    if (getrlimit(RLIMIT_NOFILE, &lim) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to get current RLIMIT_NOFILE settings.");
    }
    lim.rlim_cur = fd_limit;
    if (setrlimit(RLIMIT_NOFILE, &lim) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to set RLIMIT_NOFILE to desired value.");
    }

    for ever {
        auto update = poll.wait(span, -1);
        if (update[0].data.fd == STDIN_FILENO) {
            return; // exit upon input from stdin, this is partially for testing and partially because it's useful
        }

        if (update[0].data.fd == shut->get_fd()) {
            shut->callback();
        }
        else if (update[0].data.fd == connfact.get_fd()) {
            auto result = connfact.get_new_connection();
            if (!result.has_value()) {
                continue;
            }
            auto game_result = pending_game.add_user(result.value());
            if (game_result.has_value()) {
                handle_gamestartup(game_result.value(), pending_game, shut, randomizer, poll);
            }
            else {
                // ACTUALLY START THE GAME
                renew_game(pending_game, shut, randomizer, poll);
            }
        }
        else if (update[0].data.fd == pending_game.get_fd()) {
            handle_gamestartup(pending_game.try_ready_game(), pending_game, shut, randomizer, poll);
        }
        else {
            throw std::runtime_error("idk something went wrong");
        }
    }
    
}
