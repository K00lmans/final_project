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
std::optional<std::pair<std::string, std::string>> parse_have_card_msg(const InputBuffer<BUF_SIZE> &msgbuf, std::size_t msg_len) {
    static const std::string MSG_HDR("HAVE-CARD,");
    if (
        MSG_HDR.size() >= (msg_len - 2) || 
        msgbuf.size() < msg_len ||
        msgbuf[msg_len - 1] != '\n' ||
        msgbuf[msg_len - 2] != '\r'
    ) {
        return std::optional<std::pair<std::string, std::string>>(std::nullopt);
    }
    for (std::size_t i = 0; i < MSG_HDR.size(); ++i) {
        if (msgbuf[i] != MSG_HDR[i]) {
            return std::optional<std::pair<std::string, std::string>>(std::nullopt);
        }
    }
    std::string playername;
    std::string cardname;
    std::size_t i = MSG_HDR.size();
    while (msgbuf[i] != ',') {
        if (i == msg_len - 2) {
            return std::optional<std::pair<std::string, std::string>>(std::nullopt);
        }
        playername.push_back(msgbuf[i++]);
    }
    ++i;
    for (; i < msg_len - 2; ++i) {
        cardname.push_back(msgbuf[i]);
    }
    if (!cards::suspects_set.contains(playername)) {
        return std::optional<std::pair<std::string, std::string>>(std::nullopt);
    }
    if (!cards::cards_set.contains(cardname)) {
        return std::optional<std::pair<std::string, std::string>>(std::nullopt);
    }

    return std::optional(std::pair(playername, cardname));
}

template <std::size_t BUF_SIZE>
bool check_cards_msg(const std::string &header, const InputBuffer<BUF_SIZE> &msgbuf, std::size_t msg_len) {
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
    for (int j = 0; j < 3; ++j) {
        str.clear();
        for (; msgbuf[i] != ',' && msgbuf[i] != '\r'; ++i) {
            if (i > msg_len - 2) {
                return false;
            }
            str.push_back(msgbuf[i]);
        }
        if (!cards::cards_set.contains(str)) {
            return false;
        }
        ++i; // skip the ','
    }
    return true;
}

// presumes the message is a valid accuse-message
template <std::size_t BUF_LEN>
std::string parse_cards(const std::string &header, InputBuffer<BUF_LEN> &buf, std::size_t msg_len) {
    std::size_t i = header.size();
    std::string result;
    while (buf[i] != '\r') {
        result.push_back(buf[i++]);
    }
    return result;
}

template <std::size_t BUF_SIZE>
bool check_endturn_msg(const InputBuffer<BUF_SIZE> &buf, std::size_t msg_len) {
    static const std::string END_TURN("END-TURN\r\n");
    if (msg_len != END_TURN.size()) {
        return false;
    }
    for (std::size_t i = 0; i < msg_len; ++i) {
        if (buf[i] != END_TURN[i]) {
            return false;
        }
    }
    return true;
}

