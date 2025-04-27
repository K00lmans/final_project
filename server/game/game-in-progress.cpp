#include <socket-handling/fd-utils.hpp>
#include <game/game-in-progress.hpp>
#include <algorithm>
#include "parsing.hpp"


static const std::string CONN_ERR("A player encountered a connection error.");
static const std::string INTERNAL_ERR("An unexpected server error occurred.");
static const std::string MSG_ERR("A player sent an invalid message.");

std::optional<std::size_t> find_cardholder(const std::array<std::string, 3> &search_cards, const std::vector<Player> &players, std::size_t current_player_index) {
    for (const std::string &card : search_cards) {
        for (std::size_t i = 0; i < players.size(); ++i) {
            if (
                i != current_player_index && 
                (std::find(players[i].cards.begin(), players[i].cards.end(), card) != players[i].cards.end())
            ) {
                return std::optional(i);
            }
        }
    }
    return std::optional<std::size_t>(std::nullopt);
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


// This logic is kinda a dumpster fire tbh
// There's so many error conditions that excessive early returns are the only option if I don't want extreme nesting
// either way the code is unreadable lol
//
// good luck reading this lol




GameInProgress::GameInProgress(GameStartup &&startup) : game(std::move(startup.get_gamedata())) {
    epoll_event ev { .events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET, .data = { .fd = -1 } };
    for (Player &player : game.get_players()) {
        ev.data.fd = player.fd;
        game.mod_poll_fd(player.fd, ev);
    }
    std::string playerlist{"PLAYERS-LIST,"};
    for (Player &player : game.get_players()) {
        playerlist += player.name + ',';
    }
    broadcast(playerlist + "\r\nTURN-START," + game.get_players()[0].name + "\r\n");
    is_valid = flush_out();
    if (!is_valid) {
        send_err_msg(CONN_ERR);
    }
}

bool GameInProgress::callback() {
    std::cerr << "woo callback" << std::endl;
    epoll_event ev = game.wait_for_event(-1);
    Player &player_with_event = find_io(ev.data.fd);
    if ((ev.events & EPOLLRDHUP) || (ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        std::cerr << "wtf error" << std::endl;
        send_err_msg(CONN_ERR);
        return false;
    }
    if (ev.events & EPOLLOUT) {
        SocketStatus result(player_with_event.outbuf.flush(player_with_event.fd));
        std::cerr << "wtf" << std::endl;
        switch (result) {
        case SocketStatus::Finished:
        case SocketStatus::Blocked:
            break;
        case SocketStatus::Error:
        case SocketStatus::ZeroReturned:
        std::cerr << "wtf error" << std::endl;
            send_err_msg(CONN_ERR);
            return false;
        }
    }

    if (ev.events & EPOLLIN) {
        std::cerr << "does it get here" << std::endl;

        if (ev.data.fd == game.get_players()[turn_index].fd) {
            return current_player_msg();
        }
        else {
            return other_player_msg(ev.data.fd);
        }
    }
    return true;
}

void GameInProgress::broadcast(const std::string &message) {
    std::shared_ptr<std::string> msg(new std::string(message));
    for (Player &player : game.get_players()) {
        player.outbuf.add_message(msg);
    }
}

bool GameInProgress::flush_out() {
    bool can_continue = true;
    for (Player &player : game.get_players()) {
        switch (player.outbuf.flush(player.fd)) {
        case SocketStatus::Blocked:
        case SocketStatus::Finished:
            break;
        case SocketStatus::Error:
        case SocketStatus::ZeroReturned:
            can_continue = true;
        }
    }
    return can_continue;
}

bool GameInProgress::other_player_msg(int fd) {
    Player &player = find_io(fd);
    switch (player.inbuf.buf_read(player.fd)) {
    case SocketStatus::Error:
    case SocketStatus::ZeroReturned:
        send_err_msg(CONN_ERR);
        return false;
    default:
        break;
    }

    std::optional<std::size_t> msg_end = player.inbuf.get_msg_end();
    if (!msg_end) {
        if (player.inbuf.full()) {
            send_err_msg(MSG_ERR);
            return false; // sent too many chars
        }
        else {
            return true; // we've read all we can
        }
    }

    // now we have a valid message!
    if (!check_cards_msg("HAVE-CARD,", player.inbuf, msg_end.value(), player.name)) {
        send_err_msg(MSG_ERR);
        return false;
    }
    // now we can just send this message to every player
    std::string msg;
    for (std::size_t i = 0; i < msg_end.value(); ++i) {
        msg.push_back(player.inbuf[i]);
    }
    broadcast(msg);
    return flush_out();
}

bool GameInProgress::current_player_msg() {  
    std::cerr << "asdf" << std::endl;
    Player &current = game.get_players()[turn_index];
    switch (current.inbuf.buf_read(current.fd)) {
    case SocketStatus::Error:
    case SocketStatus::ZeroReturned:
        send_err_msg(CONN_ERR);
        return false;
    case SocketStatus::Blocked:
    case SocketStatus::Finished:
        break;
    }

    std::optional<std::size_t> msg(current.inbuf.get_msg_end());
    if (!msg.has_value()) {
        return true; // message hasn't fully sent yet
    }

    // now we definitely have something to parse, what is it?
    //

    // bug, never gets here. why?
    std::cerr << "pian" << std::endl;

    if (check_cards_msg("CARD-REQUEST,", current.inbuf, msg.value(), current.name) && turn_state == WaitingForCards) {
        turn_state = WaitingForTurnEnd;
        auto result = parse_cards("CARD-REQUEST,", current.inbuf, msg.value());
        current.inbuf.pop_front(msg.value());
        auto index = find_cardholder(result, game.get_players(), turn_index);
        std::string card_str(result[0] + ',' + result[1] + ',' + result[2]);
        if (!index.has_value()) {
            // player guessed the right cards
            broadcast("CARD-REQUEST-EMPTY," + current.name + ',' + card_str + "\r\n");
            return flush_out();
        }
        Player &found_player = game.get_players()[index.value()];
        broadcast("CARD-REQUEST," + current.name + ',' + found_player.name + ',' + card_str + "\r\n");
        return flush_out();
    }
    else if (check_cards_msg("ACCUSE,", current.inbuf, msg.value(), current.name)) {
        if (!handle_accuse(msg.value())) {
            send_err_msg(MSG_ERR);
            return false;
        }
        current.inbuf.pop_front(msg.value()); // can technically return without popping inbuf, but those are error states and will be cleaned up
    }
    else if (check_endturn_msg(current.inbuf, msg.value())) {
        current.inbuf.pop_front(msg.value());
        // do nothing, let fall through to broadcast
    }
    else {
        send_err_msg(MSG_ERR);
        return false;
    }
    broadcast("TURN-START," + game.get_players()[turn_index].name + "\r\n");
    return flush_out();
};


std::optional<std::size_t> GameInProgress::search_for_players() const {
    for (std::size_t i = 0; i < game.get_players().size(); ++i) {
        if (players_in_game[(i + turn_index) % game.get_players().size()]) {
            return std::optional((i + turn_index) % game.get_players().size());
        }
    }
    return std::optional<std::size_t>(std::nullopt);
}
bool GameInProgress::handle_accuse(std::size_t msg_end) {
    Player &current = game.get_players()[turn_index];
    auto result = parse_cards("ACCUSE,", current.inbuf, msg_end);
    for (const std::string &card : result) {
        if (!std::find(game.get_win_cards().begin(), game.get_win_cards().end(), card)) {
            broadcast("ACCUSE-FAIL," + current.name + ',' + result[0] + ',' + result[1] + ',' + result[2] + "\r\n");
            if (!flush_out()) {
                send_err_msg(CONN_ERR);
                return false;
            }
            players_in_game[turn_index] = false;
            auto search_res(search_for_players());
            if (!search_res.has_value()) {
                send_ending_msg("All players have made a false accusation.");
                return false;
            }
            turn_index = search_res.value();
            break;
        }
    }
    return true;
}
