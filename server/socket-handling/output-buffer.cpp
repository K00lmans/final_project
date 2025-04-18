#include "output-buffer.hpp"
#include <climits>
// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
//
// Description: Allows for adding "messages" to an output queue.
//              When flush() is called, as many as possible will be sent.
//
// Tags: 
//
// NOT_IMPLEMENTED
// NO_TESTS

std::optional<SocketStatus> OutputBuffer::flush(int fd) {
    if (outbuf.empty()) {
        return std::optional<SocketStatus>(std::nullopt);
    }
    int iovcnt = (outbuf.size() > IOV_MAX) ? IOV_MAX : outbuf.size();
}
