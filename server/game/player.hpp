#pragma once

//
// Maintainer: Athena Boose
//
// Description: A struct containing all the state necessary for a player.
//              This is a struct rather than a class since it's mainly intended to hold a bit of data.
//

#include <socket-handling/input-buffer.hpp>
#include <socket-handling/output-buffer.hpp>
#include "response.hpp"

struct Player {
    Player() = delete;
    Player(int fd, std::string &&character) : fd(fd), character(std::move(character)) {}

    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;

    Player(Player &&) = default;
    Player &operator=(Player &&) = default;

    InputBuffer<128> inbuf;
    OutputBuffer outbuf;
    int fd;
    std::string character;
    Response expected_response;
};
