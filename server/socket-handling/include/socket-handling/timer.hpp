#pragma once

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description:
// Provides a convenient, RAII interface to the POSIX timerfd interface.
//
// Tags:
//
// NO_TESTS
//

#include <socket-handling/fd-utils.hpp>

class Timer {
    public:
    Timer(); // creates and does not set timer
    Timer(int num_ms); // creates and sets timer
    ~Timer() { if (fd != -1) close_except(fd); }

    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;

    Timer(Timer &&timer) : fd(timer.fd) { timer.fd = -1; }
    Timer &operator=(Timer &&timer);

    // this is kinda bad, dependency injection would be better here, but at the moment I don't know whether I'll go for a classic poll-style server architecture or something with callback functions
    int get_fd() const { return fd; }
    void set(int num_ms);

    private:
    int fd;
};
