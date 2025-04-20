#pragma once

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description:
// Cleanly sends a closing message to any added sockets before properly calling shutdown() and then close() on them.
//
// NOT_IMPLEMENTED
// NO_TESTS

#include <cstddef>
#include <cstring>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <array>
#include "output-buffer.hpp"
#include "timer.hpp"
#include "fd-poll.hpp"

template <std::size_t EPOLL_BUF_SIZE = 16, std::size_t CLOSE_TIME_MS = 60000>
class Shutdown {
    public:
    Shutdown();
    ~Shutdown();

    // Makes a best-effort attempt to shutdown the socket and ensure any messages sent to it are fully sent.
    // Assuming no errors occur, any messages already sent to the socket will be fully sent before the socket is actually closed.
    void shutdown(int fd, OutputBuffer &&outbuf);
    void callback();

    private:
    enum SockState {
        SendingMsg,
        WaitingZeroRead
    };
    struct ClosingSocket {
        ClosingSocket(int sock_fd, OutputBuffer &&outbuf) : sock_fd(sock_fd), outbuf(std::move(outbuf)){}
        ClosingSocket(const ClosingSocket &) = delete;
        ClosingSocket &operator=(const ClosingSocket &) = delete;
        ClosingSocket(ClosingSocket &&) = default;
        ClosingSocket &operator=(ClosingSocket &&) = default;

        SockState state = SendingMsg;
        Timer timer;
        OutputBuffer outbuf;
        int sock_fd;
    };

    std::unordered_map<int, ClosingSocket> map;
    FdPoll poll;
    std::array<epoll_event, EPOLL_BUF_SIZE> event_list;
};


template <std::size_t EPOLL_BUF_SIZE, std::size_t CLOSE_TIME_MS>
void Shutdown<EPOLL_BUF_SIZE, CLOSE_TIME_MS>::shutdown(int fd, OutputBuffer &&outbuf) {
    try {
        ClosingSocket sock(fd, std::move(outbuf));
        SocketStatus status = sock.outbuf.flush();
        epoll_event ev{ .events = EPOLLOUT | EPOLLRDHUP, .data = {.fd = fd} };
        switch (status) {
        case SocketStatus::Finished:
            sock.status = WaitingZeroRead;
            ev.events = EPOLLIN | EPOLLRDHUP;
        case SocketStatus::Blocked:
            sock.timer.set(CLOSE_TIME_MS);
            poll.ctl(EPOLL_CTL_ADD, fd, ev);
            map.insert(std::pair(fd, std::move(sock)));
            break;
        case SocketStatus::ZeroReturned:
            throw std::system_error(0, std::generic_category(), "Write returned 0 when trying to send closing message.");
        case SocketStatus::Error:
            throw std::system_error(errno, std::generic_category(), "Encountered a write error while closing socket.");
        }
    }
    catch (const std::system_error &e) {
        std::cerr << "Failed to shutdown socket, forceclosing. More info:\n" ;
        if (e.code().value() != 0) {
            std::cerr << strerror(e.code().value()) << "\n";
        }
        std::cerr << e.what() << "\n" << std::endl;
        close_except(fd);
    }
}

template <std::size_t EPOLL_BUF_SIZE, std::size_t CLOSE_TIME_MS>
void Shutdown<EPOLL_BUF_SIZE, CLOSE_TIME_MS>::callback() {
    std::span<epoll_event> events(
        poll.wait(std::span(event_list))
    );
    for (const epoll_event &ev : events) {
        ClosingSocket &sock = map[ev.data.fd];
        // do shit
    }
}
