#include "run-server.hpp"


void handle_gamestartup(GameStartup::StartState state, GameStartup &pending_game, std::shared_ptr<Shutdown<>> shut, std::mt19937 &randomizer, FdPoll &poll) {
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
        // IMPLEMENT ACTUAL GAME CODE AND MOVE THE CORE OF THE GAME INTO THE NEW GAME THINGY
        pending_game.clear();
        break;
    }
}
