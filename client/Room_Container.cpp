/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 4-28-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "Room_Container.h"

Room_Container::Room_Container(const std::shared_ptr<sf::Texture> &empty_texture, const float sprite_size, const float x,
    const float y) {
    this->empty_texture = empty_texture;
    for (int seat = 0; seat < 6; seat++) {
        seats.push_back(sf::Sprite(*empty_texture));
        const auto current_size = seats[seat].getGlobalBounds().size;
        seats[seat].setScale({sprite_size / current_size.x, sprite_size / current_size.y});
        seats[seat].setPosition({x + (sprite_size + 1) * seat, y + (sprite_size + 1) * seat});
    }
}

void Room_Container::draw(sf::RenderWindow screen) const {
    for (const auto seat : seats) {
        screen.draw(seat);
    }
}

void Room_Container::add_player_to_seat(const sf::Texture &player_texture) {
    for (int seat = 0; seat < 6; seat++) {
        if (seats_status[seat]) {
            change_sprite_texture(seats[seat], player_texture);
            seats_status[seat] = true;
            break;
        }
    }
}

void Room_Container::remove_player_from_seat(const sf::Texture *player_texture) {
    for (int seat = 0; seat < 6; seat++) {
        if (&seats[seat].getTexture() == player_texture) {
            change_sprite_texture(seats[seat], *empty_texture);
            seats_status[seat] = false;
            break;
        }
    }
}
