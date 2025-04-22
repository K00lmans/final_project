#include <sys/timerfd.h>
#include <system_error>
#include <socket-handling/timer.hpp>


constexpr static inline itimerspec ms_to_itimerspec(int num_ms) {
    return itimerspec {
        .it_interval = {
            .tv_sec = 0,
            .tv_nsec = 0,
        },
        .it_value = {
            .tv_sec = num_ms / 1000, 
            .tv_nsec = (num_ms % 1000) * 1000000
        }
    };
}

Timer::Timer(int num_ms) : fd(timerfd_create(CLOCK_MONOTONIC, 0)) {
    if (fd == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not create timer.");
    }
    set(num_ms);
}

Timer &Timer::operator=(Timer &&timer) {
        if (fd != -1) {
            close_except(fd);
        }
        fd = timer.fd;
        timer.fd = -1;
        return *this;
    }

void Timer::set(int num_ms) {
    itimerspec tm(ms_to_itimerspec(num_ms));
    if (timerfd_settime(fd, 0, &tm, NULL) == -1) {
        throw std::system_error(errno, std::generic_category(), "Could not set timer.");
    }
}

