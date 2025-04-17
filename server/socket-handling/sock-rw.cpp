#include <cstddef>
#include <cerrno>
#include <vector>
#include "sock-rw.hpp"

constexpr std::size_t MAX_ALLOWED_EINTR = 128;

std::pair<ssize_t, SocketStatus> exhaustive_readv(int fd, const struct iovec *iov, int iovcnt) {
    int iov_index = 0;
    ssize_t total_read = 0;
    std::size_t eintr_count = 0;
    std::vector<iovec> iovs(iovcnt);
    for (int i = 0; i < iovcnt; ++i) {
        iovs.push_back(iov[i]);
    }
    while (iov_index < iovcnt) {
        ssize_t readval = readv(fd, iovs.data() + iov_index, iovcnt - iov_index);

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
        while (readval > 0) {
            if (iovs[iov_index].iov_len <= (size_t)readval) {
                readval -= iovs[iov_index].iov_len;
                total_read += iovs[iov_index].iov_len;
                ++iov_index;
            }
            else {
                iovs[iov_index].iov_base = (char *)iovs[iov_index].iov_base + readval;
                iovs[iov_index].iov_len -= readval;
                total_read += readval;
                readval = 0;
            }
        }
    }
    return std::pair(total_read, SocketStatus::FullySent);
}

std::pair<ssize_t, SocketStatus> exhaustive_writev(int fd, const struct iovec *iov, int iovcnt) {
    int iov_index = 0;
    ssize_t total_written = 0;
    std::size_t eintr_count = 0;
    std::vector<iovec> iovs(iovcnt);
    for (int i = 0; i < iovcnt; ++i) {
        iovs.push_back(iov[i]);
    }
    while (iov_index < iovcnt) {
        ssize_t writeval = writev(fd, iovs.data() + iov_index, iovcnt - iov_index);

        // check for the morbillion error states
        if (writeval == 0) {
            return std::pair(total_written, SocketStatus::ZeroRead);
        }
        if (writeval == -1 && errno == EINTR && eintr_count < MAX_ALLOWED_EINTR) {
            ++eintr_count;
            continue;
        }
        if (writeval == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return std::pair(total_written, SocketStatus::Blocked);
        }
        if (writeval == -1) {
            return std::pair(total_written, SocketStatus::Error);
        }
        while (writeval > 0) {
            if (iovs[iov_index].iov_len <= (size_t)writeval) {
                writeval -= iovs[iov_index].iov_len;
                total_written += iovs[iov_index].iov_len;
                ++iov_index;
            }
            else {
                iovs[iov_index].iov_base = (char *)iovs[iov_index].iov_base + writeval;
                iovs[iov_index].iov_len -= writeval;
                total_written += writeval;
                writeval = 0;
            }
        }
    }
    return std::pair(total_written, SocketStatus::FullySent);
}

