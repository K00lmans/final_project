#pragma once
#include <string>
#include <socket-handling/input-buffer.hpp>
#include "cards.hpp"

template <std::size_t BUF_SIZE>
bool get_player_name(const InputBuffer<BUF_SIZE> &msg_string, std::size_t msg_len, std::string &out_string) {
    static const std::string PLAYER_SELECT("PLAYER-SELECT,");
    if (
        PLAYER_SELECT.size() >= (msg_len - 2) || 
        msg_string.size() < msg_len ||
        msg_string[msg_len - 1] != '\n' ||
        msg_string[msg_len - 2] != '\r'
    ) {
        return false;
    }

    for (std::size_t i = 0; i < PLAYER_SELECT.size(); ++i) {
        if (msg_string[i] != PLAYER_SELECT[i]) {
            return false;
        }
    }
    std::string name;
    for (std::size_t i = PLAYER_SELECT.size(); i < msg_len - 2; ++i) {
        out_string.push_back(msg_string[i]);
        name.push_back(msg_string[i]);
    }
    if (!cards::suspects_set.contains(name)) {
        return false; // didn't enter a valid player
    }

    return true;
}

template <std::size_t BUF_SIZE>
bool check_have_card_msg(const InputBuffer<BUF_SIZE> &msgbuf, std::size_t msg_len) {
    static const std::string HAVE_CARD("HAVE-CARD,");
    if (
        HAVE_CARD.size() >= (msg_len - 2) || 
        msgbuf.size() < msg_len ||
        msgbuf[msg_len - 1] != '\n' ||
        msgbuf[msg_len - 2] != '\r'
    ) {
        return false;
    }

    for (std::size_t i = 0; i < HAVE_CARD.size(); ++i) {
        if (msgbuf[i] != HAVE_CARD[i]) {
            return false;
        }
    }
    std::string str;
    std::size_t i = HAVE_CARD.size();
    while (true) {
        if (i == (msg_len - 2)) {
            return false; // ran out of message
        }

        if (msgbuf[i] == ',') {
            if (cards::suspects_set.contains(str)) {
                break;
            }
            else {
                return false; // didn't enter a valid string
            }
        }
        str.push_back(msgbuf[i++]);
    }

    str.clear();
    for (; i < msg_len - 2; ++i) {
        str.push_back(msgbuf[i]);
    }
    if (cards::suspects_set.contains(str)) {
        return true;
    }
    else {
        return false;
    }
}
