#pragma once
#include <unordered_map>
#include <memory>
#include <event2/event.h>
#include "event.hpp"

class Notifier {
    public:
    void add(int fd, short what, std::shared_ptr<Event> ev);
    void remove(int fd);

};
