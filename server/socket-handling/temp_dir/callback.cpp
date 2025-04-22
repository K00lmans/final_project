#include <sys/epoll.h>
#include <cerrno>
#include <system_error>
#include <sys/timerfd.h>
#include <unistd.h>
#include <socket-handling/fd-utils.hpp>
#include <socket-handling/callback.hpp>

// NOT_IMPLEMENTED

void Callback::add(int fd, int event_type, std::shared_ptr<Event> event) {
    int op = 0;
    if (items.count(fd)) {
        op = EPOLL_CTL_MOD;
        items[fd].push_back(std::pair(event_type, event));
    }
    else {
        op = EPOLL_CTL_ADD;
        auto new_elem = std::vector<std::pair<int, std::shared_ptr<Event>>>();
        new_elem.push_back(std::pair(event_type, event));
        items.insert(std::pair(fd, std::move(new_elem)));
    }
    epoll_event ev = { .events = 0, .data = {.fd = fd} };
    if (event_type & Event::READABLE) {
        ev.events |= EPOLLIN;
    }
    if (event_type & Event::WRITABLE) {
        ev.events |= EPOLLOUT;
    }
    if (event_type & Event::CLOSED) {
        ev.events |= EPOLLRDHUP;
    }
    if (event_type & Event::EDGE_TRIGGERED) {
        ev.events |= EPOLLET;
    }
    
    errno = 0;
    if (epoll_ctl(epfd, op, fd, &ev) == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not add to epoll instance.");
    }
}

void Callback::remove(int fd) {
    if (items.count(fd)) {
        errno = 0;
        if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            throw std::system_error(errno, std::generic_category(), "Could not remove from epoll instance.");
        }
        items.erase(fd);
    }
}

void Callback::callback_until(int timeout_ms) {
    int timerfd = -1;
    if (timeout_ms > 0) {
        timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (timerfd == -1) {
            throw std::system_error(errno, std::generic_category(), "Could not create timer.");
        }

        itimerspec spec{
            .it_interval = {},
            .it_value = {
                .tv_sec = timeout_ms / 1000,
                .tv_nsec = 1000000000 * (timeout_ms % 1000)
            },
        };
        if (timerfd_settime(timerfd, 0, &spec, NULL) == -1) {
            close_except(timerfd);
            throw std::system_error(errno, std::generic_category(), "Could not set timer.");
        }
        epoll_event ev{
            .events = EPOLLIN,
            .data{.fd = timerfd},
        };
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, timerfd, &ev) == -1) {
            close_except(timerfd);
            throw std::system_error(errno, std::generic_category(), "Could not add timer to epoll.");
        }
    }
    // HAVNE NOT FINISHED


    if (timeout_ms > 0) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, timerfd, NULL);
        close_except(timerfd);
    }

}

