#include "run-server.hpp"


GameHandler::GameHandler(const std::string &port_no) : connfact(port_no) {
    epoll_event ev{.events = EPOLLIN, .data = {.fd = -1}};

    ev.data.fd = connfact.get_fd();
    poll.ctl(EPOLL_CTL_ADD, connfact.get_fd(), ev);

    ev.data.fd = shut->get_fd();
    poll.ctl(EPOLL_CTL_ADD, shut->get_fd(), ev);

    ev.data.fd = pending_game.get_fd();
    poll.ctl(EPOLL_CTL_ADD, pending_game.get_fd(), ev);
}

void GameHandler::run_event() {
    int eventfd = get_event();
    if (eventfd == shut->get_fd()) {
        shut->callback();
    }
    else if (eventfd == connfact.get_fd()) {
        auto result = connfact.get_new_connection();
        if (!result.has_value()) {
            return;
        }
        auto game_result = pending_game.add_user(result.value());
        if (game_result.has_value()) {
            handle_gamestartup(game_result.value());
        }
        else {
            // the game is full, start it
            handle_gamestartup(GameStartup::Ready);
            // then add the user
            handle_gamestartup(pending_game.add_user(result.value()).value());
        }
    }
    else if (eventfd == pending_game.get_fd()) {
        handle_gamestartup(pending_game.ready_game());
    }
    else if (running_games.contains(eventfd)) {
        if (!running_games.at(eventfd).callback()) {
            running_games.erase(eventfd);
            std::cerr << "erased game" << std::endl;
        }
    }
    else {
        throw std::runtime_error("idk something went wrong");
    }
}

void GameHandler::handle_gamestartup(GameStartup::StartState state) {
    epoll_event ev {};
    switch (state) {
    case GameStartup::NotReady:
        break;
    case GameStartup::Error:
        std::cerr << "Failed to startup game, removing it." << std::endl;
        pending_game.clear();
        break;
    case GameStartup::Ready:
        std::cerr << "Game started successfully!" << std::endl;
        int gamefd = pending_game.get_fd();
        running_games.try_emplace(gamefd, std::move(pending_game));
        if (!running_games.at(gamefd).check_validity()) {
            running_games.erase(gamefd); // just get rid of the game if it had an error
        }
        pending_game = std::move(GameStartup(shut, randomizer));
        ev.events = EPOLLIN | EPOLLERR;
        ev.data.fd = pending_game.get_fd();
        poll.ctl(EPOLL_CTL_ADD, pending_game.get_fd(), ev);
        break;
    }
}
