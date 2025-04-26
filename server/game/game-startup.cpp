#include <memory>
#include <algorithm>
#include <game/game-startup.hpp>
#include "parsing.hpp"
#include "cards.hpp"

GameStartup::GameStartup(std::shared_ptr<Shutdown<>> shutdown, std::mt19937 &randomizer) : game(shutdown, randomizer), cards_list(cards::all_cards) {
    std::shuffle(cards_list.begin(), cards_list.end(), randomizer);
}

GameStartup::StartState GameStartup::try_ready_game() {
    if (size() == 0) {
        return NotReady;
    }
    epoll_event event = game.wait_for_event(0);
    if (
        (event.events & EPOLLRDHUP) ||
        (event.events & EPOLLERR) ||
        (event.events & EPOLLHUP)
    ) {
        return Error;
    }

    if (event.events & EPOLLOUT) {
        for (Player &pl : game.get_players()) {
            if (pl.fd == event.data.fd) {
                return process_out_event(pl);
            }
        }
    }
    else if (event.events & EPOLLIN) {
        return process_in_event(game.get_players()[current_index.value()]);
    }

    return Error;
}

std::optional<GameStartup::StartState> GameStartup::add_user(int fd) {
    if (game.get_players().size() >= 6) {
        return std::optional<StartState>(std::nullopt);
    }
    game.add_player(fd);
    if (!current_index.has_value()) {
        current_index = std::optional(game.get_players().size() - 1);
        return std::optional(initialize_new_player(game.get_players()[current_index.value()]));
    }
    return std::optional(NotReady);
}

// the below code is really terrible but the logic for this just inherently sucks, so not much i can do about it lol

GameStartup::StartState GameStartup::process_in_event(Player &player) {
    std::optional<std::size_t> msg_end;
    epoll_event ev{ .events = EPOLLRDHUP | EPOLLET, .data = { .fd = player.fd } };
    switch (player.inbuf.buf_read(player.fd)) {
    case SocketStatus::Error:
    case SocketStatus::ZeroReturned:
        return Error; // can error out here because even if they sent us stuff they won't be present for the game to start!
    case SocketStatus::Finished:
    case SocketStatus::Blocked:
        msg_end = player.inbuf.get_msg_end();
        if (!msg_end.has_value()) {
            return (player.inbuf.full()) ? Error : NotReady;
        }
        
        // added their selected name to the names-string
        if (!get_player_name(player.inbuf, msg_end.value(), player.name)) {
            return Error;
        }
        picked_players_message.pop_back();
        picked_players_message.pop_back();
        picked_players_message += ',' + player.name + "\r\n";

        // remove just the input flag from the player's eventlist
        ev.events |= (player.outbuf.empty()) ? 0 : (uint32_t)EPOLLOUT;
        game.mod_poll_fd(player.fd, ev);

        if (current_index.value() == game.get_players().size() - 1) {
            current_index = std::optional<std::size_t>(std::nullopt);
            if (game.get_players().size() >= 3) {
                return Ready;
            }
            else {
                return NotReady;
            }
        }
        else {
            ++(current_index.value());
            return initialize_new_player(game.get_players()[current_index.value()]);
        }

    }
    throw std::runtime_error("how did this get here?");
}

GameStartup::StartState GameStartup::process_out_event(Player &player) {
    // just let us know when the socket breaks
    epoll_event ev{ .events = EPOLLRDHUP | EPOLLET, .data = { .fd = player.fd } };

    switch (player.outbuf.flush(player.fd)) {
    case SocketStatus::Error:
    case SocketStatus::ZeroReturned:
        return Error;
    case SocketStatus::Finished:
    case SocketStatus::Blocked:
        if (player.outbuf.empty()) {
            game.mod_poll_fd(player.fd, ev);
        }
        return NotReady;
    }
    return NotReady;
}


GameStartup::StartState GameStartup::initialize_new_player(Player &player) {
    epoll_event ev { .events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET, .data = {.fd = player.fd}};
    player.outbuf.add_message(
        std::shared_ptr<std::string>(new std::string(
            picked_players_message + "PLAYER-CARDS," + cards_list[current_card] + "," + cards_list[current_card + 1] + "," + cards_list[current_card + 2] + "\r\n"
        ))
    );
    for (int i = 0; i < 3; ++i) {
        player.cards[i] = cards_list[current_card++];
    }
    switch (player.outbuf.flush(player.fd)) {
    case SocketStatus::ZeroReturned:
    case SocketStatus::Error:
        return Error;
    case SocketStatus::Finished:
    case SocketStatus::Blocked:
        if (player.outbuf.empty()) {
            ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
            game.mod_poll_fd(player.fd, ev);
        }
        return NotReady;
    }
    return Error;
}

