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
        name.push_back(msg_string[i]);
    }
    if (!cards::suspects_set.contains(name)) {
        return false; // didn't enter a valid player
    }
    out_string += name;
    return true;
}

template <std::size_t BUF_SIZE>
bool check_cards_msg(const std::string &header, const InputBuffer<BUF_SIZE> &msgbuf, std::size_t msg_len, const std::string &playername) {
    if (
        header.size() >= (msg_len - 2) || 
        msgbuf.size() < msg_len ||
        msgbuf[msg_len - 1] != '\n' ||
        msgbuf[msg_len - 2] != '\r'
    ) {
        return false;
    }

    for (std::size_t i = 0; i < header.size(); ++i) {
        if (msgbuf[i] != header[i]) {
            return false;
        }
    }
    std::string str;
    std::size_t i = header.size();
    while (true) {
        if (i == (msg_len - 2)) {
            return false; // ran out of message
        }

        if (msgbuf[i] == ',') {
            if (str == playername) {
                break;
            }
            else {
                return false; // didn't enter a valid string
            }
        }
        str.push_back(msgbuf[i++]);
    }

    str.clear();
    for (++i; i < msg_len - 2; ++i) {
        str.push_back(msgbuf[i]);
    }
    if (cards::cards_set.contains(str)) {
        return true;
    }
    else {
        return false;
    }
}

// presumes the message is a valid accuse-message
template <std::size_t BUF_LEN>
std::array<std::string, 3> parse_cards(const std::string &header, InputBuffer<BUF_LEN> &buf, std::size_t msg_len) {
    std::size_t i = header.size();
    std::array<std::string, 3> result;
    while (buf[i++] != ',') {} // skipping player message

    while (buf[i] != ',') {
        result[0].push_back(buf[i++]);
    }
    ++i;

    while (buf[i] != ',') {
        result[1].push_back(buf[i++]);
    }
    ++i;

    while (buf[i] != '\r') {
        result[2].push_back(buf[i++]);
    }
    return result;
}
