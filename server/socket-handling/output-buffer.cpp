#include "output-buffer.hpp"
#include <climits>
// NOT_IMPLEMENTED

std::optional<SocketStatus> OutputBuffer::flush(int fd) {
    if (outbuf.empty()) {
        return std::optional<SocketStatus>(std::nullopt);
    }
    int iovcnt = (outbuf.size() > IOV_MAX) ? IOV_MAX : outbuf.size();
}
