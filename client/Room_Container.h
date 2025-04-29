/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 4-28-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 *
 *A class to handle the display of player tokens as they enter and exit rooms
 */

#ifndef ROOM_CONTAINER_H
#define ROOM_CONTAINER_H

#include <vector>
#include <SFML/Graphics.hpp>

#include "main.h" // Gets the code for changing textures

class Room_Container {
    std::vector<sf::Sprite> seats;
    bool seats_status[6] = {false, false, false, false, false, false};
    std::shared_ptr<sf::Texture> empty_texture;

public:
    Room_Container(const std::shared_ptr<sf::Texture> &empty_texture, float sprite_size, float x, float y);

    void add_player_to_seat(const sf::Texture &player_texture);

    void remove_player_from_seat(const sf::Texture *player_texture);

    void draw(sf::RenderWindow screen) const;
};

#endif //ROOM_CONTAINER_H
