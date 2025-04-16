#include <cassert>
#include "../message.hpp"

// helper function so i don't have to copy-paste the same code 10 times
static void test_message(Message &msg, const std::string &msgstr) {
    assert(msg.data() == msgstr);
    assert(msg.size() == msg.data().size());
    assert(msg.size() == msgstr.size());

    iovec iov (msg.get_iovec());
    assert(iov.iov_base == msg.data().data());
    assert(iov.iov_len == msg.size());

    msg.pop_front(msgstr.size() / 3); // pop some amount we know we can pop
    iov = msg.get_iovec();
    assert(iov.iov_base == msg.data().data() + msgstr.size() / 3);
    assert(iov.iov_len == msg.size() - msgstr.size() / 3);


}

static void test_message_example(void) {

}

static void test_message_move(void) {
}

static void test_message_move_assignment(void) {
}


int main(void) {
    test_message_example();
    return 0;
}
