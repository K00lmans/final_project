#include <memory>
#include "game-startup.hpp"
#include "parsing.hpp"

// the below code is really terrible but the logic for this just inherently sucks, so not much i can do about it lol


GameStartup::StartState GameStartup::process_current_player_event(Player &player, uint32_t event) {
    bool just_sent = false;
    if (event & EPOLLIN) {
        // the optional should always have a value, we check for full later
        auto result = player.inbuf.buf_read(player.fd);
        std::optional<std::size_t> end_index;
        switch (result) {
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
            player.outbuf.add_message(std::shared_ptr<std::string>(new std::string(picked_cards_msg)));
            just_sent = true; // now we need to go to the second part and try to flush
            if (current_index.value() == game.players.size() - 1) {
                if (game.players.size() >= 3) {
                    return Ready;
                }
                else {
                    current_index = std::nullopt;
                }
            }
            else {
                ++(current_index.value());
            }
            break;
            // afdsafdsafsda
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
            return GameStartup::NotReady;
        case SocketStatus::Finished:
            // first write finished, now we wait for the user to send us something back
            ev.data.fd = player.fd;
            ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
            game.poll.ctl(EPOLL_CTL_MOD, player.fd, ev);
            return GameStartup::NotReady; // still gotta wait for a response
        case SocketStatus::Error:
        case SocketStatus::ZeroReturned:
            return GameStartup::Error;
        }
    }
    return Error;
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
    if (size() == 0 || !current_index.has_value()) {
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
}
