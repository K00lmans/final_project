/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 4-23-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 *
 *All the helper functions, variables, and other data structures for main.cpp
 */

#ifndef MAIN_H
#define MAIN_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

#include "Scratch_Pad.h"
#include "GameCode/Token.hpp"
#include "GameCode/TokenID.hpp"
#include "GameCode/Board.hpp"
#include "GameCode/Room.hpp"
#include "GameCode/BoardSetup.hpp"
#include "GameCode/Player.hpp"

// The proper ordering of characters
typedef enum characters {
    COL_MUSTARD,
    MS_SCARLET,
    PROF_PLUM,
    MR_GREEN,
    MRS_WHITE,
    MRS_PEACOCK
} Characters;

// Global Variables
inline double window_scaler = .75;// The scaler for the y size of the window based on the users screen. Can not be 1 or greater
inline double background_size[2] = {1162.0, 1159.0}; // Size in pixels of the background image
inline sf::Vector2u screen_size;
inline sf::Font font("client/graphics/NotoSans-Black.ttf");
inline Token tokens[6] = {Token(TokenID::COL_MUSTARD), Token(TokenID::MS_SCARLET), Token(TokenID::PROF_PLUM),
    Token(TokenID::MR_GREEN), Token(TokenID::MRS_WHITE), Token(TokenID::MRS_PEACOCK)};
inline Tile starting_positions[6] = {Tile(23, 7), Tile(16, 0), Tile(0, 5),
    Tile(9, 24), Tile(14, 24), Tile(0, 18)};
inline auto rng_device = std::mt19937(std::random_device()());
inline double square_size = 42;
inline double upper_board_corner[2] = {75.5, 52};

unsigned int get_window_x_size();

void pick_characters(unique_ptr<Player> player_list[6]);

unique_ptr<sf::RectangleShape> create_standard_box();

std::vector<std::pair<sf::Text, sf::RectangleShape>> create_buttons(const unique_ptr<sf::RectangleShape> &standard);

template<typename T>
int generate_random_int(T min, T max) {
    return static_cast<int>(std::uniform_int_distribution<std::mt19937::result_type>(min, max)(rng_device));
}

std::vector<Tile> find_reachable_tiles(const Tile &position, int movement, const Board &board);

// Returns true if the item is in the vector, false otherwise
template<class T>
bool check_if_in_vector(const std::vector<T> &vector, const T &element) {
    for (auto &item : vector) {
        if (item == element) {
            return true;
        }
    }
    return false;
}

void change_sprite_texture(sf::Sprite &sprite, const sf::Texture &new_texture);

#endif //MAIN_H
