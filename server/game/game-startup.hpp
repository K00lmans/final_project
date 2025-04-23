#pragma once

#include <random>
#include <optional>
#include "game-data.hpp"
#include "cards.hpp"

// Handles startup of game
class GameStartup {
    public:
    friend class GameInProgress;
    GameStartup(std::shared_ptr<Shutdown<>> shutdown, std::mt19937 &randomizer);
    ~GameStartup() = default;

    GameStartup(const GameStartup &) = delete;
    GameStartup &operator=(const GameStartup &) = delete;

    GameStartup(GameStartup &&) = default;
    GameStartup &operator=(GameStartup &&) = default;

    enum StartState {
        Ready,
        NotReady,
        Error,
    };

    StartState try_ready_game();
    int get_fd() const { return game.poll.fd(); }
    bool add_user(int fd) {
        if (game.players.size() >= 6) {
            return false;
        }
        game.add_player(fd);
        if (!current_index.has_value()) {
            current_index = std::optional(game.players.size() - 1);
        }
        return true;
    }
    std::size_t size() const { return game.players.size(); }
    private:
    StartState process_current_player_event(Player &player, uint32_t event);
    GameData game;
    std::string picked_cards_msg{"PLAYERS-TAKEN\n"};
    std::array<std::string, 22> cards_list{cards::all_cards};
    std::optional<std::size_t> current_index{std::nullopt};
    std::size_t current_card = 0;

};
