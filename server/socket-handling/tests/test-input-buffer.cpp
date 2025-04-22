#include <cassert>
#include <socket-handling/input-buffer.hpp>
#include "sockfuncs.hpp"

static void test_input_buffer(void) {
    InputBuffer<255> buf;
    assert(buf.empty());
    assert(!buf.full());
    assert(buf.size() == 0);

    auto retval = buf.buf_read(123);
    assert(retval.has_value());
    assert(retval.value() == SocketStatus::Blocked);
    assert(buf.empty());
    assert(!buf.full());
    assert(buf.size() == 0);

    test_type = RwOption::RwZero;
    retval = buf.buf_read(123);
    assert(retval.has_value());
    assert(retval.value() == SocketStatus::ZeroReturned);
    assert(buf.empty());
    assert(!buf.full());
    assert(buf.size() == 0);

    test_type = RwOption::Err;
    retval = buf.buf_read(123);
    assert(retval.has_value());
    assert(retval.value() == SocketStatus::Error);
    assert(buf.empty());
    assert(!buf.full());
    assert(buf.size() == 0);

    test_type = RwOption::RwData;
    num_to_read = 111;
    retval = buf.buf_read(123);

    assert(retval.has_value());
    assert(retval.value() == SocketStatus::Blocked);
    assert(!buf.empty());
    assert(!buf.full());
    assert(buf.size() == 111);

    buf.pop_front(50);
    assert(!buf.empty());
    assert(!buf.full());
    assert(buf.size() == 61);

    num_to_read = 150;
    retval = buf.buf_read(123);

    assert(retval.has_value());
    assert(retval.value() == SocketStatus::Blocked);
    assert(!buf.empty());
    assert(!buf.full());
    assert(buf.size() == 61 + 150);

    num_to_read = 12343;
    retval = buf.buf_read(123);
    assert(retval.has_value());
    assert(retval.value() == SocketStatus::Finished);
    assert(!buf.empty());
    assert(buf.full());
    assert(buf.size() == 255);

    retval = buf.buf_read(124);
    assert(!retval.has_value());
    assert(!buf.empty());
    assert(buf.full());
    assert(buf.size() == 255);

    buf.pop_front(123);
    assert(!buf.empty());
    assert(!buf.full());
    assert(buf.size() == 255 - 123);

    num_to_read = 20;
    retval = buf.buf_read(123);
    assert(retval.has_value());
    assert(retval.value() == SocketStatus::Blocked);
    assert(!buf.empty());
    assert(!buf.full());
    assert(buf.size() == 255 - 123 + 20);
}

int main(void) {
    seed_rand();
    test_input_buffer();
    reset_globals();
}
