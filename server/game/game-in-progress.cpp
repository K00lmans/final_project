#include <span>
#include <socket-handling/fd-utils.hpp>
#include "game-in-progress.hpp"

bool GameInProgress::callback() {
    std::array<epoll_event, 1> event_arr;
    std::span<epoll_event> span(event_arr);
    epoll_event ev = poll.wait(span, -1)[0];
    Player &player_with_event = find_io(ev.data.fd);
    if (ev.events & EPOLLRDHUP || ev.events & EPOLLERR || ev.events & EPOLLHUP) {
        std::swap(player_with_event, players[players.size() - 1]);
        close_except(players[players.size() - 1].fd);
        players.pop_back();
        send_err_msg("Another player unexpectedly closed their connection.");
        return false;
    }

    if (ev.events & EPOLLIN) {}
    if (ev.events & EPOLLOUT) {}

    return true;
}
