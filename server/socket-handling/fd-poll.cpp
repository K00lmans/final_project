#include <cerrno>
#include <unistd.h>
#include <socket-handling/fd-poll.hpp>
#include <socket-handling/fd-utils.hpp>
#include <socket-handling/fatal-error.hpp>

#include <execinfo.h>
#include <iostream>

//
// Maintainer: Athena Boose
//
// Description: A handy, RAII wrapper for an epoll instance.
//
// Tags:
//
// NO_TESTS

FdPoll::FdPoll() {
    errno = 0;
    epfd = epoll_create1(0);
    if (epfd == -1) {
        throw FatalError(errno, "Could not create epoll instance.");
    }
}

FdPoll::~FdPoll() {
    if (epfd != -1) {
        close_except(epfd);
    }
}

FdPoll::FdPoll(FdPoll &&poll) : epfd(poll.epfd) {
    poll.epfd = -1;
}

FdPoll &FdPoll::operator=(FdPoll &&poll) {
    if (&poll != this) {
        if (epfd != -1) {
            close_except(epfd);
        }
        epfd = poll.epfd;
        poll.epfd = -1;
    }
    return *this;
}

void FdPoll::ctl(int op, int fd, epoll_event &event) {
    errno = 0;
    if (epoll_ctl(epfd, op, fd, &event) == -1) {
        std::cerr << fd << std::endl;
        throw FatalError(errno, "Could not run epoll_ctl on epoll instance.");
    }
}

std::span<epoll_event> FdPoll::wait(const std::span<epoll_event> &events, int timeout) {
    errno = 0;
    int retval = epoll_wait(epfd, events.data(), events.size(), timeout);
    if (retval == -1) {
        throw FatalError(errno, "Could not wait on epoll instance.");
    }
    return std::span(events.first(retval));
}

