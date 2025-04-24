#pragma once
#include <string>
#include <socket-handling/input-buffer.hpp>

template <std::size_t BUF_SIZE>
bool get_player_name(const InputBuffer<BUF_SIZE> &msg_string, std::size_t msg_len, std::string &out_string) {
    const std::string PLAYER_NAME("PLAYER-SELECT,");
    out_string.pop_back();
    out_string.pop_back();
    if (PLAYER_NAME.size() >= msg_len || msg_string.size() < msg_len) {
        return false;
    }
    if (msg_string[msg_len - 1] != '\n') {
        return false;
    }
    if (msg_string[msg_len - 2] != '\r') {
        return false;
    }

    for (std::size_t i = 0; i < PLAYER_NAME.size(); ++i) {
        if (msg_string[i] != PLAYER_NAME[i]) {
            return false;
        }
    }
    for (std::size_t i = PLAYER_NAME.size(); i < msg_len; ++i) {
        out_string.push_back(msg_string[i]);
    }
    return true;
}
