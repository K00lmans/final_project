#pragma once
#include <vector>
#include <deque>
#include <memory>
#include "message.hpp"
#include "input-buffer.hpp"

class Socket;

// A connection with a fixed-size input buffer.
// This is intended for use in applications where "messages" from client to server have a defined max size; otherwise, other buffering systems might be more efficient.
template <std::size_t MAX_SIZE, std::size_t MAX_EINTR = 128>
class Connection {
    public:
    Connection() = delete;
    Connection(evutil_socket_t fd) : fd(fd) {}

    // Adds a message to the pending list of messages associated with this connection.
    // This does not actually attempt to send the message, however subsequent calls to flush_send() will send it.
    constexpr void add_to_send(std::shared_ptr<Message> msg) { outbuf.push_back(msg); }

    // Flushes the pending list of messages that are waiting to be sent.
    // If a fatal error occurs, -1 will be returned, and you should allow this connection to terminate.
    // Otherwise, the amount of bytes written to the socket will be returned.
    //
    // Any messages that can't be sent (due to EWOULDBLOCK/EAGAIN getting triggered) will remain in the message list.
    //
    // After this function exits, you are guaranteed that there is either no more data left to write, that the socket has blocked, or that there has been an unrecoverable error.
    // Connection::send_buf_empty can be used to check for the former two; if the buffer is empty it's guaranteed the socket hasn't blocked.
    ssize_t flush_send();

    // you should empty the buffer as much as possible before using this!
    //
    // Reads from the associated file descriptor until the input buffer fills, the fd blocks, or there is an error.
    // If a fatal error occurs, -1 will be returned, and you should allow this connection to terminate.
    // If the attempt to receive succeeds, the total amount of data in the input buffer will be returned.
    // You can check whether the input buffer is full using the Connection::recv_buf_full method.
    ssize_t receive() noexcept;

    // removes the first `amount` bytes from the recvbuffer
    constexpr void recv_buf_pop(std::size_t amount) noexcept { inbuf.remove_front(amount); }
    constexpr std::size_t recv_buf_size() const noexcept { return inbuf.size(); } // gets size of internal recv buffer
    constexpr bool recv_buf_full() const noexcept { return inbuf.full(); }

    constexpr bool send_buf_empty() const noexcept { return outbuf.empty(); }

    constexpr char &operator[](std::size_t index) noexcept { return inbuf[index]; }
    constexpr const char &operator[](std::size_t index) const noexcept { return inbuf[index]; }

    evutil_socket_t get_fd() const { return fd; }

    // have add to poll fn
    private:
    InputBuffer<MAX_SIZE> inbuf;
    std::deque<std::shared_ptr<Message>> outbuf;
    evutil_socket_t fd;
};

// the following is nested to hell and i hate it
// i'll try to think of a better way to do this

template <std::size_t MAX_SIZE, std::size_t MAX_EINTR>
ssize_t Connection<MAX_SIZE, MAX_EINTR>::flush_send() {
    std::vector<iovec> iovecs;
    const std::size_t max_iters = (outbuf.size() < IOV_MAX) ? outbuf.size() : IOV_MAX;
    for (std::size_t i = 0; i < max_iters; ++i) {
        iovecs.push_back(outbuf[i]->get_iovec());
    }
    size_t first_iovec_index = 0;
    size_t num_eintr = 0;
    ssize_t total_amt_sent = 0;

    while (!outbuf.empty()) {
        errno = 0;
        size_t bytes_written = writev(fd, &(iovecs[first_iovec_index]), iovecs.size() - first_iovec_index);
        if (bytes_written == -1) {
            if (errno == EINTR && num_eintr < MAX_EINTR) {
                ++num_eintr; // a signal interrupted us, just try again
                // this will eventually fail if interrupted enough times
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return total_amt_sent; // connection is blocking, so edge-triggered async io will give another notifiation
            }
            else {
                return -1; // socket had some sort of error, anyone who wants to know what it is can check errno
            }
        }

        total_amt_sent += bytes_written;

        while (bytes_written > 0) {
            if (outbuf.front()->size() <= bytes_written) {
                bytes_written -= outbuf.front()->size();
                ++first_iovec_index;
                outbuf.pop_front();
            }
            else {
                // cursed function naming - this doesn't remove anything from outbuf, it removes the bytes we don't care about from outbuf.front()
                outbuf.front()->pop_front(bytes_written);
                iovecs[first_iovec_index] = outbuf.front()->get_iovec();
                bytes_written = 0;
            }
        }
    }
    return total_amt_sent;
}

template <std::size_t MAX_SIZE, std::size_t MAX_EINTR>
ssize_t Connection<MAX_SIZE, MAX_EINTR>::receive() noexcept {
    ssize_t amount_read = 0;
    size_t num_eintr = 0;
    while (!inbuf.full()) {
        ssize_t temp = inbuf.append_read(fd).value();
        if (temp == 0) {
            return 0;
        }
        if (temp == -1) {
            if (errno == EINTR && num_eintr < MAX_EINTR) {
                ++num_eintr; // a signal interrupted us, just try again
                // this will eventually fail if interrupted enough times
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return inbuf.size(); // connection is blocking, so edge-triggered async io will give another notifiation
            }
            else {
                return -1; // socket had some sort of error, anyone who wants to know what it is can check errno
            }
        }
    }
    return inbuf.size();
}

