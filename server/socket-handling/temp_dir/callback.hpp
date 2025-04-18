#pragma once

// NOT_IMPLEMENTED

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description:
// Calls a given callback functor whenever a desired event occurs on a file descriptor.
//

#include <unordered_map>
#include <vector>
#include <memory>


class Callback;


// inherit from this and define your own callback() function
// it's entirely valid to edit the caller from within this callback function, so it's passed in as a parameter
class Event {
    public:
    static constexpr int READABLE = 0b1;
    static constexpr int WRITABLE = 0b10;
    static constexpr int CLOSED = 0b100;
    static constexpr int ERR = 0b1000;
    static constexpr int EDGE_TRIGGERED = 0b10000;
    virtual bool callback(Callback &caller, int condition) = 0;
};

class Callback {
    public:
    void add(int fd, int event_type, std::shared_ptr<Event> event);
    void remove(int fd);
    void try_run_callbacks() { callback_until(0); }
    void callback_until(int timeout_ms); // if timeout = -1, will loop forever, if timeout = 0, will only loop once

    int get_fd() const { return epfd; }

    private:
    int epfd;
    std::unordered_map<
        int, 
        std::vector<
            std::pair<
                int /*event type*/, std::shared_ptr<Event>
            >
        > 
    > items;
};

