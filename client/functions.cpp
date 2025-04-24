/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-23-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "main.h"

unsigned int get_window_x_size(const sf::Vector2u size) {
    return static_cast<unsigned int>(((size.y * window_scaler) * (background_size[0] / background_size[1])));
}

void pick_characters(Player *player_list[6]) {
    std::vector<int> players_not_picked;
    for (int player = 0; player < 6; player++) {
        if (player_list[player] == nullptr) {
            players_not_picked.push_back(player);
        }
    }
}
