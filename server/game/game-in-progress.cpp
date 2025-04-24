#include <span>
#include <socket-handling/fd-utils.hpp>
#include "game-in-progress.hpp"

static void delete_bad_player(std::vector<Player> &players, Player &bad_player) {
    Player &good_player = players[players.size() - 1];
    std::swap(bad_player, good_player);
    close_except(players[players.size() - 1].fd);
    players.pop_back();
}

bool GameInProgress::callback() {
    std::array<epoll_event, 1> event_arr;
    std::span<epoll_event> span(event_arr);
    epoll_event ev = game.poll.wait(span, -1)[0];
    Player &player_with_event = find_io(ev.data.fd);
    if (ev.events & EPOLLRDHUP || ev.events & EPOLLERR || ev.events & EPOLLHUP) {
        delete_bad_player(game.players, player_with_event);
        send_err_msg("Another player unexpectedly closed their connection.");
        return false;
    }

    if (ev.events & EPOLLIN) {} // suffering :)
    if (ev.events & EPOLLOUT) {
        SocketStatus result(player_with_event.outbuf.flush(player_with_event.fd));
        switch (result) {
        case SocketStatus::Finished:
        case SocketStatus::Blocked:
            break;
        case SocketStatus::Error:
        case SocketStatus::ZeroReturned:
            delete_bad_player(game.players, player_with_event);
            send_err_msg("Another player unexpectedly closed their connection.");
            return false;
        }
    }

    return true;
}
