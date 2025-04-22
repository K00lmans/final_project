#pragma once

class Game {
    public:
    private:
    std::vector<Player> players;
    std::array<std::string, 3> win_cards;
    FdPoll poll;
    std::shared_ptr<Shutdown<>> shutdown_singleton;
};
