#pragma once
#include <cstddef>
#include <event2/event.h>
#include <optional>
#include <array>
#include <sys/uio.h>


#include <iostream>

template <size_t buffer_size>
class InputBuffer {
    private:
    static constexpr inline bool is_power_of_two(std::size_t val) {
        return val && !(val & (val - 1)); // see http://www.graphics.stanford.edu/~seander/bithacks.html
    }

    static_assert(is_power_of_two(buffer_size), "The size of a RingBuffer must be a power of 2."); // this is premature optimization but i love bitwise operators :3
    public:
    constexpr InputBuffer() noexcept : buffer{}, buffer_full(false), start(0), end(0) {}

    constexpr InputBuffer(InputBuffer &&buf) = default;
    constexpr InputBuffer &operator=(InputBuffer &&buf) = default;

    constexpr char &operator[](const size_t index) noexcept { return buffer[mod(start + index)]; }
    constexpr const char &operator[](const size_t index) const noexcept { return buffer[mod(start + index)]; }

    constexpr size_t size() const noexcept {
        if (buffer_full) {
            return buffer_size;
        }
        return (end >= start) ? (end - start) : (buffer_size - (end - start));
    }
    constexpr bool empty() const noexcept { return !buffer_full && start == end; }
    constexpr bool full() const noexcept { return buffer_full; }

    std::optional<ssize_t> append_read(evutil_socket_t fd) noexcept {
        if (buffer_full) {
            return std::optional<ssize_t>(std::nullopt);
        }
        ssize_t retval = 0;
        if (end == start) {
            end = 0;
            start = 0;
            retval = read(fd, buffer.data(), buffer_size);
        }
        else if (end < start) {
            retval = read(fd, buffer.data() + end, start - end);
        }
        else if (start == 0) { // implies end > start, since end != start
            retval = read(fd, buffer.data() + end, buffer_size - end);
        }
        else {
            std::array<iovec, 2> iovecs = {
                iovec { .iov_base = buffer.data() + end, .iov_len = buffer_size - end },
                iovec { .iov_base = buffer.data(), .iov_len = start } 
            };
            std::cout << "readamt0=" << buffer_size - end << ",readamt1="<< start << std::endl;
            retval = readv(fd, iovecs.data(), 2);
        }

        if (retval == -1 || retval == 0) {
            return std::optional(retval);
        }
        end = mod(end + retval);
        if (start == end) {
            buffer_full = true;
        }
        return std::optional(retval);
    }

    constexpr void remove_front(size_t amount) noexcept {
        start = mod(start + amount);
        if (amount != 0) {
            buffer_full = false;
        }
    }

    private:
    constexpr InputBuffer(const InputBuffer &) = default;
    constexpr InputBuffer &operator=(const InputBuffer &) = default;

    constexpr static size_t mod(const size_t val) noexcept { return val & (buffer_size - 1); } // again, premature optimization

    std::array<char, buffer_size> buffer;
    bool buffer_full;
    size_t start;
    size_t end;
};
