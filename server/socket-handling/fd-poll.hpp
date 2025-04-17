#pragma once

// NO_TESTS

//
// Maintainer: Athena Boose
//
// Description: A handy, RAII wrapper for an epoll instance.
//

#include <sys/epoll.h>
#include <vector>
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
    int wait(std::span<epoll_event> &events, int timeout);

    // convenience overloads
    int wait(std::vector<epoll_event> &events, int timeout) {
        std::span<epoll_event> span(events.begin(), events.size());
        return wait(span, timeout);
    }
    template <std::size_t nel>
    int wait(std::array<epoll_event, nel> &events, int timeout) {
        std::span<epoll_event> span(events.begin, events.size());
        return wait(span, timeout);
    }
    private:
    int epfd;
};
