#pragma once


// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description:
// Provides a convenient, RAII interface to the POSIX timerfd interface.
//
// Tags:
//
// NOT_IMPLEMENTED
// NO_TESTS
//


class Timer {
    public:
    Timer(); // creates and does not set timer
    Timer(int num_ms); // creates and sets timer
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
