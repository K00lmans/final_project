#include "timer.hpp"
#include <sys/timerfd.h>


constexpr static inline timespec ms_to_timespec(int num_ms) {
    return {
        .tv_sec = num_ms / 1000, 
        .tv_nsec = (num_ms % 1000) * 1000000000
    };
}

Timer::Timer() {
}

Timer::Timer(int num_ms) {
}

Timer::~Timer() {
}

Timer::Timer(Timer &&timer) {
}

