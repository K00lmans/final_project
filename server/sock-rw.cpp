#include <cstddef>
#include <cerrno>
#include "sock-rw.hpp"

constexpr std::size_t MAX_ALLOWED_EINTR = 128;


std::pair<ssize_t, SocketStatus> exhaustive_readv(int fd, const struct iovec *iov, int iovcnt) {
    int iov_index = 0;
    ssize_t total_read = 0;
    std::size_t eintr_count = 0;
    while (iov_index < iovcnt) {
        ssize_t readval = readv(fd, iov + iov_index, iovcnt - iov_index);

        // check for the morbillion error states
        if (readval == 0) {
            return std::pair(total_read, SocketStatus::ZeroRead);
        }
        if (readval == -1 && errno == EINTR && eintr_count < MAX_ALLOWED_EINTR) {
            ++eintr_count;
            continue;
        }
        if (readval == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return std::pair(total_read, SocketStatus::Blocked);
        }
        if (readval == -1) {
            return std::pair(total_read, SocketStatus::Error);
        }
    }
}

std::pair<ssize_t, SocketStatus> exhaustive_writev(int fd, const struct iovec *iov, int iovcnt);
