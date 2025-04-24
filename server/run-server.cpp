#include "run-server.hpp"


void renew_game(GameStartup &pending_game, std::shared_ptr<Shutdown<>> shut, std::mt19937 &randomizer, FdPoll &poll) {
    pending_game = GameStartup(shut, randomizer);
    epoll_event ev {.events = EPOLLIN | EPOLLERR, .data = {.fd = pending_game.get_fd()}};
    poll.ctl(EPOLL_CTL_ADD, ev.data.fd, ev);

}
