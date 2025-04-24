#pragma once

//
// Maintainer: Athena Boose
//
// Description: A struct containing all the state necessary for a player.
//              This is a struct rather than a class since it's mainly intended to hold a bit of data.
//

#include <string>
#include <socket-handling/input-buffer.hpp>
#include <socket-handling/output-buffer.hpp>

struct Player {
    Player() = delete;
    Player(int fd) : fd(fd) {}

    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;

    Player(Player &&) = default;
    Player &operator=(Player &&) = default;

    InputBuffer<128> &get_input_buffer() { return inbuf; }
    const InputBuffer<128> &get_input_buffer() const { return inbuf; }

    InputBuffer<128> inbuf;
    OutputBuffer outbuf;
    int fd;
};
