#include "game.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <array>

constexpr std::size_t TEMP_BUF_SIZE = 256;
constexpr std::size_t EPOLL_ARR_SIZE = 16;

Game::~Game() {
    for (Player &player : players) {
        close(player.fd); 
    }
    if (poll_fd != -1) {
        close(poll_fd);
    }
}


bool Game::add_user(int fd) {
    errno = 0;
    epoll_event ev = {
        .events = EPOLLIN | EPOLLOUT | EPOLLRDHUP, 
        .data = { .fd = fd }
    };
    return (epoll_ctl(poll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) ? false : true;
}

bool Game::send_shutdown_message() {
    bool success = true;
    for (Player &player: players) {
        if (player.state == Open) {
            if (shutdown(player.fd, SHUT_WR) == 0) {
                player.state = WriteShutdown;
            }
            else {
                success = false;
            }
        }
    }
    return success;
}

bool Game::shutdown_callback() {
    std::array<struct epoll_event, EPOLL_ARR_SIZE> epoll_arr;
    std::array<char, TEMP_BUF_SIZE> readbuf;
    int num_shutdown = epoll_wait(poll_fd, epoll_arr.data(), EPOLL_ARR_SIZE, 0);
    for (int i = 0; i < num_shutdown; ++i) {
        ssize_t readval = read(epoll_arr[i].data.fd, readbuf.data(), TEMP_BUF_SIZE);
    }
}

