#pragma once


//
// Maintainer: Athena Boose
//
// Description: A handy, RAII wrapper for an epoll instance.
//
// Tags:
//
// NO_TESTS

#include <sys/epoll.h>
#include <span>

class FdPoll {
    public:
    FdPoll();
    ~FdPoll();

    FdPoll(const FdPoll &) = delete;
    const FdPoll &operator=(const FdPoll &) = delete;

    FdPoll(FdPoll &&poll);
    FdPoll &operator=(FdPoll &&poll);

    void ctl(int op, int fd, epoll_event &event);

    // Will never return -1 (instead throws exception)
    std::span<epoll_event> wait(std::span<epoll_event> events, int timeout);

    private:
    int epfd;
};
