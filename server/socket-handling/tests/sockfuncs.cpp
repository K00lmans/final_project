#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <cassert>
#include "sockfuncs.hpp"


RwOption test_type = RwOption::RwData;

size_t num_to_read = 0;
size_t total_num_reads = 0;


size_t num_to_write = 0;
size_t total_num_writes = 0;

size_t times_to_throw_eintr = 0;

void seed_rand(void) {
    srand(time(NULL));
}
void reset_globals(void) {
    num_to_read = 0;
    total_num_reads = 0;

    num_to_write = 0;
    total_num_writes = 0;

    test_type = RwOption::RwData;

    times_to_throw_eintr = 0;
}

// validates that a valid buffer is passed to read/write
// you gotta use valgrind to detect invalid reads/writes tho
static void validate_space(void *space, size_t size) {
    volatile char temp_space = '\0';
    char *data = (char *)space;
    for (size_t i = 0; i < size; ++i) {
        temp_space = data[i];
        data[i] = temp_space;
    }
}

extern "C" {
ssize_t read(int fd, void *buf, size_t count) {
    ++total_num_reads;
    validate_space(buf, count); // validate space first, the entire buf should be valid regardless if we use it

    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (test_type == RwOption::RwZero) {
        return 0;
    }
    if (test_type == RwOption::Err) {
        errno = EBADF;
        return -1;
    }


    size_t read_amt = 0;
    size_t max_readable = (count > num_to_read) ? num_to_read : count;
    if (num_to_read == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    if (test_type == RwOption::RwData) {
        read_amt = max_readable;
    }
    else {
        read_amt = rand() % max_readable + 1;
    }
    num_to_read -= read_amt;
    assert(read_amt > 0);
    return read_amt;
}

ssize_t write(int fd, void *buf, size_t count) {
    ++total_num_writes;
    validate_space(buf, count); // validate space first, the entire buf should be valid regardless if we use it
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (test_type == RwOption::RwZero) {
        return 0;
    }
    if (test_type == RwOption::Err) {
        errno = EBADF;
        return -1;
    }

    size_t write_amt = 0;
    size_t max_writeable = (count > num_to_write) ? num_to_write : count;
    if (num_to_write == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    if (test_type == RwOption::RwData) {
        write_amt = max_writeable;
    }
    else {
        write_amt = rand() % max_writeable + 1;
    }

    num_to_write -= write_amt;
    assert(write_amt > 0);
    return write_amt;
}


ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    ++total_num_reads;
    size_t total = 0;
    for (int i = 0; i < iovcnt; ++i) {
        validate_space(iov[i].iov_base, iov[i].iov_len);
        total += iov[i].iov_len;
    }
    if (total == 0) {
        return 0;
    }
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (test_type == RwOption::RwZero) {
        return 0;
    }
    if (test_type == RwOption::Err) {
        errno = EBADF;
        return -1;
    }

    size_t max_readable = (total > num_to_read) ? num_to_read : total;
    size_t read_amt = 0;
    if (num_to_read == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    if (test_type == RwOption::RwData) {
        read_amt = max_readable;
    }
    else {
        read_amt = rand() % max_readable + 1;
    }
    num_to_read -= read_amt;
    assert(read_amt > 0);
    return read_amt;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    ++total_num_writes;
    size_t total = 0;
    for (int i = 0; i < iovcnt; ++i) {
        validate_space(iov[i].iov_base, iov[i].iov_len);
        total += iov[i].iov_len;
    }
    if (total == 0) {
        return 0;
    }
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (test_type == RwOption::RwZero) {
        return 0;
    }
    if (test_type == RwOption::Err) {
        errno = EBADF;
        return -1;
    }


    size_t write_amt = 0;
    size_t max_writeable = (total > num_to_write) ? num_to_write : total;
    if (num_to_write == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    if (test_type == RwOption::RwData) {
        write_amt = max_writeable;
    }
    else {
        write_amt = rand() % max_writeable + 1;
    }

    num_to_write -= write_amt;
    assert(write_amt > 0);
    return write_amt;
}

}
