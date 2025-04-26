#include <socket-handling/fd-utils.hpp>
#include <game/game-in-progress.hpp>
#include "parsing.hpp"


const std::string CONN_ERR("Another player encountered a connection error.");
const std::string INTERNAL_ERR("An unexpected server error occurred.");


// Generally I try to make my early returns a bit more sensible than this
// However, this logic was (a) very nested and (b) makes the most sense when put in a linear fashion, not broken up into functions
//
// so many error conditions ;-;

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
            send_err_msg("Another player sent a message that was too long.");
            return false; // sent too many chars
        }
        else {
            return true; // we've read all we can
        }
    }

    // now we have a valid message!
    if (!check_have_card_msg(player.inbuf, msg_end.value(), player.name)) {
        send_err_msg("Another player sent an invalid message.");
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


bool GameInProgress::callback() {
    epoll_event ev = game.wait_for_event(-1);
    Player &player_with_event = find_io(ev.data.fd);
    if ((ev.events & EPOLLRDHUP) || (ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        send_err_msg(CONN_ERR);
        return false;
    }
    if (ev.events & EPOLLOUT) {
        SocketStatus result(player_with_event.outbuf.flush(player_with_event.fd));
        switch (result) {
        case SocketStatus::Finished:
        case SocketStatus::Blocked:
            return true;
        case SocketStatus::Error:
        case SocketStatus::ZeroReturned:
            send_err_msg(CONN_ERR);
            return false;
        }
    }

    if (!(ev.events & EPOLLIN)) {
        send_err_msg(INTERNAL_ERR);
        return false; // epoll returned something weird, just close everything
    }

    if (ev.data.fd == game.get_players()[turn_index].fd) {
    }
    else {
        return other_player_msg(ev.data.fd);
    }
}
