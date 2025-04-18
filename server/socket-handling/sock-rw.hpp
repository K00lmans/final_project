#pragma once
#include <sys/uio.h>
#include <utility>
// NO_TESTS


// This file contains "exhaustive" versions of the standard unix read/write functions.
// These functions will write until one of the following occurs:
//
// - The entire buffer they were given has been written
// - The socket would have blocked with EAGAIN/EWOULDBLOCK
// - The socket encountered some sort of fatal error (check errno to see what exactly)
//
// All functions return a std::pair containing the amount of data sent, combined with a status code.
// It's ugly but it works; it communicates all information about socket status up the chain.
//
//
// At present, exhaustive_read and exhaustive_write literally just call exhaustive_readv/writev because I couldn't be bothered to duplicate code.

enum class SocketStatus {
    FullySent,
    Blocked,
    ZeroRead,
    Error,
};

std::pair<ssize_t, SocketStatus> exhaustive_readv(int fd, const struct iovec *iov, int iovcnt);
std::pair<ssize_t, SocketStatus> exhaustive_writev(int fd, const struct iovec *iov, int iovcnt);

inline std::pair<ssize_t, SocketStatus> exhaustive_read(int fd, void *buf, size_t len) {
    struct iovec iov = {
        .iov_base = buf,
        .iov_len = len,
    };
    return exhaustive_readv(fd, &iov, 1);
}

inline std::pair<ssize_t, SocketStatus> exhaustive_write(int fd, void *buf, size_t len) {
    struct iovec iov = {
        .iov_base = buf,
        .iov_len = len,
    };
    return exhaustive_writev(fd, &iov, 1);
}

