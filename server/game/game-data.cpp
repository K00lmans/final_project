#include <game/game-data.hpp>
#include "cards.hpp"


using intdist = std::uniform_int_distribution<std::mt19937::result_type>;
GameData::GameData(std::shared_ptr<Shutdown<>> shutdown, std::mt19937 &randomizer) : win_cards{
    cards::suspects[intdist(0, 5)(randomizer)],
    cards::weapons[intdist(0, 6)(randomizer)],
    cards::rooms[intdist(0, 8)(randomizer)]
}, shutdown_singleton(shutdown) {}

GameData &GameData::operator=(GameData &&data) {
        for (Player &player : players) {
            shutdown_singleton->shutdown_sock(player.fd, std::move(player.outbuf));
        }
        players = std::move(data.players);
        poll = std::move(data.poll);
        win_cards = std::move(data.win_cards);
        shutdown_singleton = std::move(data.shutdown_singleton);
        return *this;
}

void GameData::clear() {
        epoll_event ev;
        for (Player &player : players) {
            poll.ctl(EPOLL_CTL_DEL, player.fd, ev);
            shutdown_singleton->shutdown_sock(player.fd, std::move(player.outbuf));
        }
        players.clear();
        // win_cards will just be reset the next time someone uses this

}
