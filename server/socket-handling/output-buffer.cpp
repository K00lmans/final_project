#include "output-buffer.hpp"
#include <climits>
#include <vector>

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description: Allows for adding "messages" to an output queue.
//              When flush() is called, as many as possible will be sent.
//
// Tags: 
//
// NO_TESTS


// Returns a std::pair containing the number of iovecs to pop off the front of the buffer, followed by the proper start index into the next one.
static std::pair<std::size_t, std::size_t> get_iovecs_to_remove(const std::vector<iovec> &iovecs, size_t bytes_written) {
    std::size_t iovecs_to_remove = 0;
    std::size_t bytes_removed = 0;
    for (const iovec &iov : iovecs) {
        if (bytes_removed + iov.iov_len > bytes_written) {
            return std::pair(iovecs_to_remove, bytes_written - bytes_removed);
        }
        ++iovecs_to_remove;
        bytes_removed += iov.iov_len;
    }
    return std::pair(iovecs.size(), 0);
}


std::optional<SocketStatus> OutputBuffer::flush(int fd) {
    if (outbuf.empty()) {
        return std::optional<SocketStatus>(std::nullopt);
    }
    int iovcnt = (outbuf.size() > IOV_MAX) ? IOV_MAX : outbuf.size();
    std::vector<iovec> iovecs;
    if (iovcnt > 0) {
        iovecs.push_back(iovec {
            .iov_base = outbuf[0]->data() + start_index,
            .iov_len = outbuf[0]->size() - start_index
        });
    }
    for (int i = 1; i < iovcnt; ++i) {
        iovecs.push_back(iovec {
            .iov_base = outbuf[i]->data(),
            .iov_len = outbuf[i]->size()
        });
    }
    auto result = exhaustive_writev(fd, iovecs.data(), iovecs.size());
    auto needs_removal = get_iovecs_to_remove(iovecs, result.first);
    for (std::size_t i = 0; i < needs_removal.first; ++i) {
        outbuf.pop_front();
    }
    start_index = needs_removal.second;
    return result.second;
}
