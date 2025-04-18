#pragma once
#include <optional>
#include <array>
#include <climits>
#include "sock-rw.hpp"

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
// 
// Description: A ring buffer for queueing input messages.
//              This is intended to be used in situations where messages have a fixed, maximum size.
//              For maximum efficiency, set the size to be one less than a power of 2 so the compiler can optimize modulus to bitwise-AND
//
//              This implementation kinda sucks, but it does at least seem to work.
//
// Tags:
//
// NO_TESTS
//

template <std::size_t buffer_size>
class InputBuffer {
    static_assert(IOV_MAX >= 2); // if somehow IOV_MAX is lower than 2, this should not compile
    public:
    constexpr InputBuffer() noexcept {}

    constexpr InputBuffer(InputBuffer &&buf) = default;
    constexpr InputBuffer &operator=(InputBuffer &&buf) = default;

    constexpr char &operator[](const size_t index) noexcept { return buffer[mod(start + index)]; }
    constexpr const char &operator[](const size_t index) const noexcept { return buffer[mod(start + index)]; }

    constexpr std::size_t empty() const { return start == end; }
    constexpr std::size_t full() const { return mod(end + 1) == start;}
    constexpr std::size_t size() const {
        if (empty()) {
            return 0;
        }
        if (full()) {
            return buffer_size;
        }
        return mod(end + truesize() - start);
    }


    std::optional<SocketStatus> buf_read(int fd) {
        // this kinda just bruteforces all the different states a ringbuffer can be in
        // it's not elegant but I had a hard time thinking about this and covering all edge cases

        // check for full buffer
        if (full()) {
            return std::optional<SocketStatus>(std::nullopt);
        }

        std::array<iovec, 2> iov{};
        int iovcnt = 0;

        if (empty()) { // buffer is empty
            start = 0; 
            end = 0;
            iov[0].iov_len = truesize() - 1;
            iov[0].iov_base = buffer.data();
            iovcnt = 1;
        }
        else if (start > end) {
            iov[0].iov_len = start - end - 1;
            iov[0].iov_base = iov.data() + end;
            iovcnt = 1;
        }
        else if (start == 0) {
            iov[0].iov_len = truesize() - end - 1; // leave an empty spot at buffer end
            iov[0].iov_base = end;
            iovcnt = 1;
        }
        else if (start == 1) {
            iov[0].iov_len = truesize() - end;
            iov[0].iov_base = end; // fill it all up, we have space
            iovcnt = 1;
        }
        else {
            iov[0].iov_len = truesize() - end;
            iov[0].iov_base = end;
            iov[1].iov_len = start - 1;
            iov[1].iov_base = iov.data();
            iovcnt = 2;
        }
        auto retval = exhaustive_readv(fd, iov.data(), iovcnt);
        if (retval.first == -1 || retval.first == 0) {
            return retval.second;
        }
        end = mod(end + retval.first);
        return retval.second;
    }
    void pop_front(std::size_t amount) {
        // should assert that amount <= size()!!!
        start = mod(start + amount);
    }

    private:
    constexpr InputBuffer(const InputBuffer &) = default;
    constexpr InputBuffer &operator=(const InputBuffer &) = default;

    constexpr static std::size_t mod(const std::size_t val) noexcept { return val % (truesize()); }
    constexpr static std::size_t truesize() noexcept { return buffer_size + 1; }

    std::array<char, buffer_size + 1> buffer;
    std::size_t start = 0; // points to the first space in the buffer with an element in it
    std::size_t end = 0; // points to the last space in the buffer with an element in it
};
