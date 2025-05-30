#include <socket-handling/tests/sockfuncs.hpp>
#include <cassert>
#include "../parsing.hpp"

static constexpr std::size_t GOOD_BUF_SIZE = 127;

// helpers

static void init_input_buf(InputBuffer<GOOD_BUF_SIZE> &buf, std::size_t readnum) {
    // creating new buffer
    buf = std::move(InputBuffer<GOOD_BUF_SIZE>());
    reset_globals();
    num_to_read = readnum;
    SocketStatus result = buf.buf_read(12345);
    assert(result == SocketStatus::Finished || result == SocketStatus::Blocked);
    reset_globals();
}


// tests
static void test_get_player_name(void) {
    InputBuffer<GOOD_BUF_SIZE> ibuf;
    std::string test("PLAYER-SELECT,garbage name\r\n");
    std::string out;
    init_input_buf(ibuf, test.size());
    for (std::size_t i = 0; i < test.size(); ++i) {
        ibuf[i] = test[i];
    }
    assert(get_player_name(ibuf, ibuf.size(), out) == false);
    assert(out.empty());

    init_input_buf(ibuf, test.size());
    test = "THIS IS A RANDOM STRING LOL\r\n";
    for (std::size_t i = 0; i < test.size(); ++i) {
        ibuf[i] = test[i];
    }
    assert(get_player_name(ibuf, ibuf.size(), out) == false);
    assert(out.empty());

    test = "no endline";
    init_input_buf(ibuf, test.size());
    for (std::size_t i = 0; i < test.size(); ++i) {
        ibuf[i] = test[i];
    }
    assert(get_player_name(ibuf, ibuf.size(), out) == false);
    assert(out.empty());

    
    test = "PLAYER-SELECT,Miss Scarlet\r\n";
    init_input_buf(ibuf, test.size());
    for (std::size_t i = 0; i < test.size(); ++i) {
        ibuf[i] = test[i];
    }
    assert(get_player_name(ibuf, ibuf.size(), out) == true);
    assert(out == "Miss Scarlet");

}

static bool cards_check_testrunner(const std::string &msg) {
    InputBuffer<GOOD_BUF_SIZE> buf;
    init_input_buf(buf, msg.size());
    for (std::size_t i = 0; i < msg.size(); ++i) {
        buf[i] = msg[i];
    }
    return check_cards_msg("CARD-REQUEST,", buf, msg.size());
}

static void test_check_cards_msg(void) {
    assert(cards_check_testrunner("garbage header,Mrs. White,Study,Rope\r\n") == false);
    assert(cards_check_testrunner("CARD-REQUEST,Study,Rope\r\n") == false);
    assert(cards_check_testrunner("CARD-REQUEST,,Mrs. White,Rope\r\n") == false);
    assert(cards_check_testrunner("CARD-REQUEST,dasfdafdsaf,Mrs. White,Study\r\n") == false);
    assert(cards_check_testrunner("CARD-REQUEST,Mrs. White,Study,Rope\r\n") == true);
}

static bool end_turn_testrunner(const std::string &msg) {
    InputBuffer<GOOD_BUF_SIZE> buf;
    init_input_buf(buf, msg.size());
    for (std::size_t i = 0; i < msg.size(); ++i) {
        buf[i] = msg[i];
    }
    return check_endturn_msg(buf, msg.size());
}

static void test_end_turn_msg(void) {
    assert(end_turn_testrunner("not an end turn msg") == false);
    assert(end_turn_testrunner("not an end turn msg with newlines\r\n") == false);
    assert(end_turn_testrunner("END-TURN,followed by nonsense\r\n") == false);
    assert(end_turn_testrunner("sameleng\r\n") == false);
    assert(end_turn_testrunner("END-TURN\r\n") == true);
}


// definitely need more tests but i don't have time to add them lol
int main(void) {
    seed_rand();
    reset_globals();
    test_get_player_name();
    reset_globals();
    test_check_cards_msg();
    reset_globals();
    test_end_turn_msg();
    reset_globals();
    return EXIT_SUCCESS;
}
