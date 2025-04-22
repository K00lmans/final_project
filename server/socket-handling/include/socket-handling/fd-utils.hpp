#pragma once
#include <sys/uio.h>
#include <utility>

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// This file contains versions of standard unix file descriptor manipulation functions that have been made neater to work with in some way.
//
// close_except() will attempt to close a file descriptor, throwing an exception if this fails.
// If interrupted by a signal, it will retry the close operation up to some reasonable maximum number of attempts, at which point it'll throw an exception.
//
// exhaustive_* functions will write until one of the following occurs:
//
// - The entire buffer they were given has been written
// - The socket would have blocked with EAGAIN/EWOULDBLOCK
// - The socket encountered some sort of fatal error (check errno to see what exactly)
//
// exhaustive_* functions return a std::pair containing the amount of data sent/read, combined with a status code in the form of a SocketStatus enum.
// The amount of data sent will be 0 if no data was sent (maybe 0 was returned, maybe an error happened, maybe the socket blocked immediately) or it will be some positive number. It's only a ssize_t because that's what readv/writev/read/write return.
//
// To find out the state of the socket, you must check the SocketStatus enum.
// This is a bit clunky, but since most of these states (a.) need to be handled separately and (b.) aren't exceptional, a status code was a better choice than an enum. (If only C++ had proper sum types [std::variant is less clear here imo])
//
// It's ugly but it works; it communicates all information about socket status up the chain.
//
//
// At present, exhaustive_read and exhaustive_write literally just call exhaustive_readv/writev because I couldn't be bothered to duplicate code.


void close_except(int fd);

enum class SocketStatus {
    Finished,
    Blocked,
    ZeroReturned,
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

