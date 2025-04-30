#pragma once

#include <random>
#include <optional>
#include <algorithm>
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

    StartState ready_game();
    int get_fd() const { return game.get_fd(); }
    std::optional<StartState> add_user(int fd);
    std::size_t size() const { return game.get_players().size(); }
    GameData &get_gamedata() { return game; }
    void clear() {
        game.clear();
        picked_players_message = "PLAYERS-TAKEN\r\n";
        current_index = std::optional<std::size_t>(std::nullopt);
    }
    void reset(std::mt19937 &randomizer) {
        game.clear();
        picked_players_message = "PLAYERS-TAKEN\r\n";
        std::shuffle(cards_list.begin(), cards_list.end(), randomizer);
        current_index = std::optional<std::size_t>(std::nullopt);
    }
    private:
    StartState process_in_event(Player &player);
    StartState process_out_event(Player &player);
    StartState initialize_new_player(Player &player);
    void add_player_cards() {
        const std::size_t gamesize = game.get_players().size();
        auto &win_cards = game.get_win_cards();
        for (std::size_t i = 0, player_counter = 0; i < cards_list.size(); ++i) {
            if (std::find(win_cards.begin(), win_cards.end(), cards_list[i]) == win_cards.end()) {
                std::size_t index = gamesize - (player_counter++ % gamesize) - 1;
                game.get_players()[index].cards.push_back(cards_list[i]);
            }
        }
    }
    bool flush_player_cards();

    GameData game;
    std::string picked_players_message{"PLAYERS-TAKEN\r\n"};
    std::array<std::string, 22> cards_list;
    std::optional<std::size_t> current_index{std::nullopt};

};
