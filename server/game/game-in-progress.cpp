#include <socket-handling/fd-utils.hpp>
#include <game/game-in-progress.hpp>

bool GameInProgress::callback() {
    epoll_event ev = game.wait_for_event(-1);
    Player &player_with_event = find_io(ev.data.fd);
    if ((ev.events & EPOLLRDHUP) || (ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        send_err_msg("Another player unexpectedly closed their connection.");
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
            send_err_msg("Another player unexpectedly closed their connection.");
            return false;
        }
    }

    if (ev.events & EPOLLIN) {
        if (ev.data.fd != game.get_players()[turn_index].fd) {
            Player &player = find_io(ev.data.fd);
            player.inbuf.buf_read(player.fd);
            std::optional<std::size_t> msg_end = player.inbuf.get_msg_end();
            if (!msg_end) {
                return true; // read all we can
            }
        }
    } // suffering :)
}
