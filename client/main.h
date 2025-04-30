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

#include "Room_Container.h"
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

// Translates the name of the room to a number for use in arrays or vectors. This is also an ordering of rooms so to speak
inline std::unordered_map<std::string, int> room_numbers = {
    {"Study", 0},
    {"Hall", 1},
    {"Lounge", 2},
    {"Library", 3},
    {"Billiard Room", 4},
    {"Conservatory", 5},
    {"Ballroom", 6},
    {"Kitchen", 7},
    {"Dining Room", 8}
};

// Global Variables
// Game Data
inline Token tokens[6] = {Token(TokenID::COL_MUSTARD), Token(TokenID::MS_SCARLET), Token(TokenID::PROF_PLUM),
    Token(TokenID::MR_GREEN), Token(TokenID::MRS_WHITE), Token(TokenID::MRS_PEACOCK)};
inline Tile starting_positions[6] = {Tile(23, 7), Tile(16, 0), Tile(0, 5),
    Tile(9, 24), Tile(14, 24), Tile(0, 18)};
inline auto rng_device = std::mt19937(std::random_device()());

// Window Information
inline double window_scaler = .75;// The scaler for the y size of the window based on the users screen. Can not be 1 or greater
inline double background_size[2] = {1162.0, 1159.0}; // Size in pixels of the background image
inline sf::Vector2u screen_size; // Is set in main
inline double square_size = 42; // Size of square for sprite
inline double upper_board_corner[2] = {75.5, 52}; // Start of where squares should tile

// Textures
// Can't be const because changed in main
inline auto background_image = make_shared<sf::Texture>(sf::Texture("client/graphics/clue_board.jpg"));
inline const shared_ptr<const sf::Texture> character_textures[6] = {
    make_shared<const sf::Texture>(sf::Texture("client/graphics/colonel_mustard.jpg")),
    make_shared<const sf::Texture>(sf::Texture("client/graphics/miss_scarlet.jpg")),
    make_shared<const sf::Texture>(sf::Texture("client/graphics/professor_plum.jpg")),
    make_shared<const sf::Texture>(sf::Texture("client/graphics/mr_green.jpg")),
    make_shared<const sf::Texture>(sf::Texture("client/graphics/mrs_white.jpg")),
    make_shared<const sf::Texture>(sf::Texture("client/graphics/mrs_peacock.jpg"))};
inline const auto empty_texture = make_shared<sf::Texture>(sf::Texture("client/graphics/blank.png"));
inline const auto selection_texture = make_shared<sf::Texture>(sf::Texture("client/graphics/selection.png"));
inline sf::Font font("client/graphics/NotoSans-Black.ttf");

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

// The way to remove an item from a vector by default is kinda weird, so here is this function instead
template<class T>
void remove_item_from_vector(std::vector<T> &vector, const T &element) {
    std::vector<T> new_vector;
    for (const auto &item : vector) {
        if (item != element) {
            new_vector.push_back(item);
        }
    }
    vector = new_vector;
}

void setup_board_sprites(std::unique_ptr<sf::Sprite> sprites[24][25]);

bool check_if_in_room(const Tile &location, const Board &board);

std::optional<Room> get_current_room(const Tile &location, const Board &board);

#endif //MAIN_H
