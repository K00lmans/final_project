#include <cerrno>
#include <cassert>
#include "sockfuncs.hpp"

// yo dawg i heard you like test cases so i put test cases in your test cases so you can test cases while you test cases
// idk what there really is to say besides this is cursed lol

void test_read() {
    char test_buf[1234];
    errno = 0;
    num_to_read = 0;
    assert(read(1234, test_buf, 1234) == -1);
    assert(total_num_reads == 1);
    assert(errno == EWOULDBLOCK || errno == EAGAIN);
    
    num_to_read = 123;
    assert(read(1234, test_buf, 1234) == 123);
    assert(total_num_reads == 2);

    test_type = RwOption::RwRand;
    assert(read(1234, test_buf, 1234) != 0); // not much we really can test for tbh
    assert(total_num_reads == 3);

    errno = 0;
    test_type = RwOption::Err;
    assert(read(1234, test_buf, 1234) == -1);
    assert(errno != EWOULDBLOCK && errno != EAGAIN && errno != 0);
    assert(total_num_reads == 4);

    errno = 0;
    test_type = RwOption::RwData;
    times_to_throw_eintr = 1;
    assert(read(1234, test_buf, 1234) == -1);
    assert(errno == EINTR);
    assert(times_to_throw_eintr == 0);
    assert(total_num_reads == 5);
}

void test_write() {
    char test_buf[1234];
    errno = 0;
    num_to_write = 0;
    assert(write(1234, test_buf, 1234) == -1);
    assert(total_num_writes == 1);
    assert(errno == EWOULDBLOCK || errno == EAGAIN);
    
    num_to_write = 123;
    assert(write(1234, test_buf, 1234) == 123);
    assert(total_num_writes == 2);

    test_type = RwOption::RwRand;
    assert(write(1234, test_buf, 1234) != 0); // not much we really can test for tbh
    assert(total_num_writes == 3);

    errno = 0;
    test_type = RwOption::Err;
    assert(write(1234, test_buf, 1234) == -1);
    assert(errno != EWOULDBLOCK && errno != EAGAIN && errno != 0);
    assert(total_num_writes == 4);

    errno = 0;
    test_type = RwOption::RwData;
    times_to_throw_eintr = 1;
    assert(write(1234, test_buf, 1234) == -1);
    assert(errno == EINTR);
    assert(times_to_throw_eintr == 0);
    assert(total_num_writes == 5);
}


int main(void) {
    test_read();
    test_write();
    // no tests for test_readv and test_writev since their logic is basically identical
}
