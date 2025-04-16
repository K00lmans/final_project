#pragma once
// NOT_IMPLEMENTED
// NO_TESTS

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description:
// Cleanly sends a closing message to any added sockets before properly calling shutdown() and then close() on them.
//

#include <cstddef>
#include <unordered_map>
#include <memory>
#include <event2/event.h>
#include "message.hpp"

template <std::size_t EPOLL_BUF_SIZE = 16, std::size_t CLOSE_TIME_MS = 60000>
class Shutdown {
    public:
    Shutdown();
    ~Shutdown();

    // Gracefully shuts down the socket. 
    // Assuming no errors occur, any messages already sent to the socket will be fully sent before the socket is actually closed
    void shutdown(evutil_socket_t fd);
    void shutdown(evutil_socket_t fd, std::shared_ptr<Message> closing_msg);

    private:
    enum SockState {
        SendingMsg,
        WaitingZeroRead
    };
    struct Socket {
        Socket(int sock_fd) : sock_fd(sock_fd) {}
        SockState state = SendingMsg;
        int sock_fd;
        int timer_fd;
    };
    int epfd;
};


