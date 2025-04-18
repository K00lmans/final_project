#pragma once

// NOT_IMPLEMENTED
// NO_TESTS

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description:
// Provides a convenient, RAII interface to the POSIX timerfd interface.
//
//

#include <sys/timerfd.h>


constexpr static inline timespec ms_to_timespec(int num_ms) {
    return {
        .tv_sec = num_ms / 1000, 
        .tv_nsec = (num_ms % 1000) * 1000000000
    };
}

class Timer {
    public:
    Timer();
    ~Timer();

    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;

    Timer(Timer &&timer);
    Timer &operator=(Timer &&timer);

    int get_fd() const;
    int set(int num_ms);

    private:
    int fd;
};
