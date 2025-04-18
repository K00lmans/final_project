#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include "sock-rw.hpp"

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description: Allows for adding "messages" to an output queue.
//              When flush() is called, as many as possible will be sent.
//
// Tags: 
//
// NOT_IMPLEMENTED
// NO_TESTS


class OutputBuffer {
    public:
    OutputBuffer() = default;

    // Adds a message to the send queue.
    // This does not ever send a message, to do that you must call flush().
    void add_message(std::shared_ptr<std::string> msg) { outbuf.push_back(msg); }

    std::optional<SocketStatus> flush(int fd);

    private:
    std::deque<std::shared_ptr<std::string>> outbuf;
    std::size_t start_index = 0;
};
