#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <socket-handling/fd-utils.hpp>

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description: Allows for adding "messages" to an output queue.
//              When flush() is called, as many as possible will be sent.
//
// Tags: 
//
// NO_TESTS


class OutputBuffer {
    public:
    OutputBuffer() = default;

    OutputBuffer(const OutputBuffer &) = delete;
    OutputBuffer &operator=(const OutputBuffer &) = delete;

    OutputBuffer(OutputBuffer &&) = default;
    OutputBuffer &operator=(OutputBuffer &&) = default;

    // Adds a message to the send queue.
    // This does not ever send a message, to do that you must call flush().
    void add_message(std::shared_ptr<std::string> msg) { outbuf.push_back(msg); }

    SocketStatus flush(int fd);
    bool empty() const { return outbuf.empty(); }

    private:

    // replace with something better that takes less memory
    std::deque<std::shared_ptr<std::string>> outbuf;
    std::size_t start_index = 0;
};
