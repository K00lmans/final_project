#include <memory>
#include "game-startup.hpp"
#include "parsing.hpp"

// the below code is really terrible but the logic for this just inherently sucks, so not much i can do about it lol

static GameStartup::StartState flush_propagate_error(Player &player, FdPoll &poll) {
    epoll_event ev{ .events = EPOLLRDHUP, .data = {.fd = player.fd}};
    switch (player.outbuf.flush(player.fd)) {
    case SocketStatus::Error:
    case SocketStatus::ZeroReturned:
        return GameStartup::Error;
    case SocketStatus::Finished:
        poll.ctl(EPOLL_CTL_MOD, player.fd, ev);
    case SocketStatus::Blocked:
        return GameStartup::Ready;
    }
    return GameStartup::Error;
}

GameStartup::StartState GameStartup::process_current_player_event(Player &player, uint32_t event) {
    bool just_sent = false;
    if (event & EPOLLIN) {
        // the optional should always have a value, we check for full later
        std::optional<std::size_t> end_index;
        switch (player.inbuf.buf_read(player.fd)) {
        case SocketStatus::Blocked:
        case SocketStatus::Finished:
            end_index = player.inbuf.get_msg_end();
            if (player.inbuf.full() && !end_index.has_value()) {
                // don't have a message, buffer is full, this is an error
                return Error;
            }
            if (!end_index.has_value()) {
                return NotReady;
            }
            picked_cards_msg[picked_cards_msg.size() - 1] = ',';
            if (!get_player_name(player.inbuf, end_index.value(), picked_cards_msg)) {
                return Error;
            }
            // sending cards message
            player.outbuf.add_message(std::shared_ptr<std::string>(new std::string(
            "PLAYER-CARDS," + cards_list[current_card] + "," + cards_list[current_card + 1] + "," + cards_list[current_card + 2] + "\r\n"
            )));
            current_card += 3;

            if (flush_propagate_error(player, game.poll) == Error) {
                return Error;
            }

            // don't need to advance to the next player
            if (current_index.value() == game.players.size() - 1) {
                if (game.players.size() >= 3) {
                    return Ready;
                }
                else {
                    current_index = std::nullopt;
                    return NotReady;
                }
            }

            // need to advance to the next player
            ++(current_index.value());
            game.players[current_index.value()].outbuf.add_message(std::shared_ptr<std::string>(new std::string(picked_cards_msg)));
            if (flush_propagate_error(game.players[current_index.value()], game.poll) == Error) {
                return Error;
            }
            return NotReady;
        case SocketStatus::ZeroReturned:
        case SocketStatus::Error:
            return Error;
        }
    }
    if (event & EPOLLOUT || just_sent) {
        auto result = player.outbuf.flush(player.fd);
        epoll_event ev;
        switch (result) {
        case SocketStatus::Blocked:
            return NotReady;
        case SocketStatus::Finished:
            // first write finished, now we wait for the user to send us something back
            ev.data.fd = player.fd;
            ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
            game.poll.ctl(EPOLL_CTL_MOD, player.fd, ev);
            return NotReady; // still gotta wait for a response
        case SocketStatus::Error:
        case SocketStatus::ZeroReturned:
            return Error;
        }
    }
    return Error;
}

GameStartup::StartState GameStartup::process_other_player_event(Player &player) {
    // always gonna be EPOLLOUT
    epoll_event ev{ .events = EPOLLRDHUP, .data = { .fd = player.fd } };

    switch (player.outbuf.flush(player.fd)) {
    case SocketStatus::Error:
    case SocketStatus::ZeroReturned:
        return Error;
    case SocketStatus::Finished:
        game.poll.ctl(EPOLL_CTL_MOD, player.fd, ev);
    case SocketStatus::Blocked:
        return NotReady;
    }
    return NotReady;
}

GameStartup::GameStartup(std::shared_ptr<Shutdown<>> shutdown, std::mt19937 &randomizer) : game(shutdown) {
    std::uniform_int_distribution<std::mt19937::result_type> suspects_dist(0, 5);
    std::uniform_int_distribution<std::mt19937::result_type> weapons_dist(0, 6);
    std::uniform_int_distribution<std::mt19937::result_type> rooms_dist(0, 8);

    game.win_cards[0] = cards::suspects[suspects_dist(randomizer)];
    game.win_cards[1] = cards::weapons[weapons_dist(randomizer)];
    game.win_cards[2] = cards::rooms[rooms_dist(randomizer)];
    
    std::shuffle(cards_list.begin(), cards_list.end(), randomizer);
}

GameStartup::StartState GameStartup::try_ready_game() {
    // must deal with shit thingy 
    if (size() == 0) {
        return NotReady;
    }
    std::array<epoll_event, 1> ep_ev;
    auto result = game.poll.wait(std::span(ep_ev), 0);
    if (
        result[0].events & EPOLLRDHUP ||
        result[0].events & EPOLLERR ||
        result[0].events & EPOLLHUP
    ) {
        return Error;
    }
    // this not done yeet
}
