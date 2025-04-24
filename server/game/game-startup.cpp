#include <memory>
#include "game-startup.hpp"
#include "parsing.hpp"

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
        if (!get_player_name(player.inbuf, msg_end.value(), picked_players_message)) {
            return Error;
        }

        // remove just the input flag from the player's eventlist
        ev.events |= (player.outbuf.empty()) ? 0 : (uint32_t)EPOLLOUT;
        game.poll.ctl(EPOLL_CTL_MOD, player.fd, ev);

        if (current_index.value() == game.players.size() - 1) {
            current_index = std::optional<std::size_t>(std::nullopt);
            if (game.players.size() >= 3) {
                return Ready;
            }
            else {
                return NotReady;
            }
        }

    }
    return Error; // clangd isn't smart enough to know that the above switch handles all cases
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
            game.poll.ctl(EPOLL_CTL_MOD, player.fd, ev);
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
    current_card += 3;
    switch (player.outbuf.flush(player.fd)) {
    case SocketStatus::ZeroReturned:
    case SocketStatus::Error:
        return Error;
    case SocketStatus::Finished:
    case SocketStatus::Blocked:
        if (player.outbuf.empty()) {
            ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        }
        game.poll.ctl(EPOLL_CTL_ADD, player.fd, ev);
        return NotReady;
    }
    return Error;
}

std::optional<GameStartup::StartState> GameStartup::add_user(int fd) {
    if (game.players.size() >= 6) {
        return std::optional<StartState>(std::nullopt);
    }
    game.add_player(fd);
    if (!current_index.has_value()) {
        current_index = std::optional(game.players.size() - 1);
        return std::optional(initialize_new_player(game.players[current_index.value()]));
    }
    return std::optional(NotReady);
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
    std::span<epoll_event> result = game.poll.wait(std::span(ep_ev), 0);
    if (
        (result[0].events & EPOLLRDHUP) ||
        (result[0].events & EPOLLERR) ||
        (result[0].events & EPOLLHUP)
    ) {
        return Error;
    }

    if (result[0].events & EPOLLOUT) {
        for (Player &pl : game.players) {
            if (pl.fd == result[0].data.fd) {
                return process_out_event(pl);
            }
        }
    }
    else if (result[0].events & EPOLLIN) {
        return process_in_event(game.players[current_index.value()]);
    }

    return Error;
}
