#include <array>
#include <cstdlib>
#include <cassert>
#include "../sock-rw.hpp"
#include "sockfuncs.hpp"

void test_exhaustive_readv(void) {
    std::array<iovec, 4> iovs {
        iovec{.iov_base = malloc(1234), .iov_len = 1234},
        iovec{.iov_base = malloc(567), .iov_len = 567},
        iovec{.iov_base = malloc(890), .iov_len = 890},
        iovec{.iov_base = malloc(111), .iov_len = 111}
    };
    auto result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 0);
    assert(result.second == SocketStatus::Blocked);

    test_type = RwOption::Err;

    result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 0);
    assert(result.second == SocketStatus::Error);

    test_type = RwOption::RwZero;

    result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 0);
    assert(result.second == SocketStatus::ZeroRead);

    test_type = RwOption::RwRand;
    num_to_read = 14321;

    result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 1234 + 567 + 890 + 111);
    assert(result.second == SocketStatus::Finished);
    
    test_type = RwOption::RwData;
    num_to_read = 14321;

    result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 1234 + 567 + 890 + 111);
    assert(result.second == SocketStatus::Finished);

    num_to_read = 143;

    result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 143);
    assert(result.second == SocketStatus::Blocked);

    times_to_throw_eintr = 42;
    test_type = RwOption::RwRand;
    num_to_read = 13423;

    result = exhaustive_readv(123, iovs.data(), 4);
    assert(result.first == 1234 + 567 + 890 + 111);
    assert(result.second == SocketStatus::Finished);

    free(iovs[0].iov_base);
    free(iovs[1].iov_base);
    free(iovs[2].iov_base);
    free(iovs[3].iov_base);
}

void test_exhaustive_writev(void) {
    std::array<iovec, 4> iovs {
        iovec{.iov_base = malloc(1234), .iov_len = 1234},
        iovec{.iov_base = malloc(567), .iov_len = 567},
        iovec{.iov_base = malloc(890), .iov_len = 890},
        iovec{.iov_base = malloc(111), .iov_len = 111}
    };
    auto result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 0);
    assert(result.second == SocketStatus::Blocked);

    test_type = RwOption::Err;

    result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 0);
    assert(result.second == SocketStatus::Error);

    test_type = RwOption::RwZero;

    result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 0);
    assert(result.second == SocketStatus::ZeroRead);

    test_type = RwOption::RwRand;
    num_to_write = 14321;

    result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 1234 + 567 + 890 + 111);
    assert(result.second == SocketStatus::Finished);
    
    test_type = RwOption::RwData;
    num_to_write = 14321;

    result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 1234 + 567 + 890 + 111);
    assert(result.second == SocketStatus::Finished);
    
    num_to_write = 143;

    result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 143);
    assert(result.second == SocketStatus::Blocked);

    times_to_throw_eintr = 42;
    test_type = RwOption::RwRand;
    num_to_write = 13423;

    result = exhaustive_writev(123, iovs.data(), 4);
    assert(result.first == 1234 + 567 + 890 + 111);
    assert(result.second == SocketStatus::Finished);

    free(iovs[0].iov_base);
    free(iovs[1].iov_base);
    free(iovs[2].iov_base);
    free(iovs[3].iov_base);
}


int main(void) {
    seed_rand();
    reset_globals();
    test_exhaustive_readv();
    reset_globals();
    test_exhaustive_writev();
    reset_globals();
}
