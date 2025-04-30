#pragma once
#include <sys/types.h>
#include <sys/uio.h>

// symbols defined here will overwrite symbols from libc when everything is linked together
// this effectively replaces read/write/readv/writev with userspace versions that just check for invalid memory accesses, proper error handling, etc.
// to ensure that these functions are equivalent to those they replace it's necessary to have a pile of globals that can be modified for testing purposes
// also, these tests really ought to be run with valgrind to detect invalid memory accesses
//
// idk whether this will work when compiled with MSVC, but it's not my fault MSVC sucks

#ifndef HIDE_READ_WRITE
extern "C" {

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
}

#endif

void seed_rand(void);
void reset_globals(void);


enum class RwOption {
    RwData,
    RwZero,
    RwRand,
    Err,
};

// defaults to RwData
extern RwOption test_type;


// initialize this with a value, run one of the above functions
// the user will be allowed to read up until they call a read with more than this value, at which point they'll do a partial read and will from then on get EWOULDBLOCK
//
// if this is 0, either EWOULDBLOCK or EAGAIN will be returned
extern size_t num_to_read; 
// this counter is incremented by 1 whenever read() or readv() is called
extern size_t total_num_reads;
// makes reads have random, positive sizes

extern size_t num_to_write; // same as num_to_read but for writes
extern size_t total_num_writes; // same as total_num_reads but for writes

// if this value is 0, nothing happens
// if this value is greater than 0, it is decremented on a call to read/write/readv/writev and EINTR is thrown
// this takes precedence over cause_read_error/cause_write_error
extern size_t times_to_throw_eintr;

