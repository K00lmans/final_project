#pragma once
#include <cstddef>
#include <string>
#include <sys/uio.h>

class Message {
    public:
    Message() = delete;
    Message(std::string &&str) : str(std::move(str)) {}
    Message(const Message &) = delete;
    Message &operator=(const Message &) = delete;

    Message(Message &&) = default;
    Message &operator=(Message &&) = default;
    
    std::string &data() { return str; }
    const std::string &data() const { return str; }

    iovec get_iovec() { return iovec {.iov_base = str.data() + start_index, .iov_len = str.size() - start_index}; }
    std::size_t size() const { return str.size(); }
    void pop_front(std::size_t amount) { start_index += amount; }
    private:
    std::string str;
    std::size_t start_index = 0;
};
