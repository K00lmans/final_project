#include <cerrno>
#include <unistd.h>
#include <system_error>
#include "fd-poll.hpp"

FdPoll::FdPoll() {
    errno = 0;
    if (epfd == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not create epoll instance.");
    }
}

FdPoll::~FdPoll() {
    if (epfd != -1) {
        close(epfd);
    }
}

FdPoll::FdPoll(FdPoll &&poll) : epfd(poll.epfd) {
    poll.epfd = -1;
}

FdPoll &FdPoll::operator=(FdPoll &&poll) {
    if (&poll != this) {
        if (epfd != -1) {
            close(epfd);
        }
        epfd = poll.epfd;
        poll.epfd = -1;
    }
    return *this;
}

void FdPoll::ctl(int op, int fd, epoll_event &event) {
    errno = 0;
    if (epoll_ctl(epfd, op, fd, &event) == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not run epoll_ctl on epoll instance.");
    }
}

int FdPoll::wait(std::span<epoll_event> &events, int timeout) {
    errno = 0;
    int retval = epoll_wait(epfd, events.data(), events.size(), timeout);
    if (retval == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not wait on epoll instance.");
    }
    return retval;
}

