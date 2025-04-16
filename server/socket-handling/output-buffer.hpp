#pragma once
// NOT_IMPLEMENTED
// NO_TESTS

#include <deque>
#include <memory>
#include <optional>
#include "message.hpp"
#include "sock-rw.hpp"


class OutputBuffer {
    public:
    OutputBuffer() = default;

    // Adds a message to the send queue.
    // This does not ever send a message, to do that you must call flush().
    void add_message(std::shared_ptr<Message> msg) { outbuf.push_back(msg); }

    std::optional<SocketStatus> flush(int fd);

    private:
    std::deque<std::shared_ptr<Message>> outbuf;
    std::size_t start_index = 0;
};
