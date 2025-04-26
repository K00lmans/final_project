#pragma once

#include <random>
#include <optional>
#include "game-data.hpp"

// Handles startup of game
class GameStartup {
    public:
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
    int get_fd() const { return game.get_fd(); }
    std::optional<StartState> add_user(int fd);
    std::size_t size() const { return game.get_players().size(); }
    void get_gamedata(GameData &data) {
        data = std::move(game);
    }
    private:
    StartState process_in_event(Player &player);
    StartState process_out_event(Player &player);
    StartState initialize_new_player(Player &player);

    GameData game;
    std::string picked_players_message{"PLAYERS-TAKEN\r\n"};
    std::array<std::string, 22> cards_list;
    std::optional<std::size_t> current_index{std::nullopt};
    std::size_t current_card = 0;

};
