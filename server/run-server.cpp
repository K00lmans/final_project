#include "run-server.hpp"


void renew_game(GameStartup &pending_game, std::shared_ptr<Shutdown<>> shut, std::mt19937 &randomizer, FdPoll &poll) {
    pending_game = std::move(GameStartup(shut, randomizer));
    epoll_event ev {.events = EPOLLIN | EPOLLERR, .data = {.fd = pending_game.get_fd()}};
    poll.ctl(EPOLL_CTL_ADD, ev.data.fd, ev);

}

void handle_gamestartup(GameStartup::StartState state, GameStartup &pending_game, std::shared_ptr<Shutdown<>> shut, std::mt19937 &randomizer, FdPoll &poll) {
    epoll_event ev {};
    switch (state) {
    case GameStartup::NotReady:
        break;
    case GameStartup::Error:
        std::cerr << "Failed to startup game, removing it." << std::endl;
        poll.ctl(EPOLL_CTL_DEL, pending_game.get_fd(), ev);
        renew_game(pending_game, shut, randomizer, poll);
        break;
    case GameStartup::Ready:
        std::cerr << "Game started successfully!" << std::endl;
        // IMPLEMENT ACTUAL GAME CODE AND MOVE THE CORE OF THE GAME INTO THE NEW GAME THINGY
        poll.ctl(EPOLL_CTL_DEL, pending_game.get_fd(), ev);
        renew_game(pending_game, shut, randomizer, poll);
        break;
    }
}
