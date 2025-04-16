/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-16-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "Scratch_Pad.h"

Scratch_Pad::Scratch_Pad(sf::Vector2u window_size) {
    window = sf::RenderWindow(sf::VideoMode({window_size.x / 4, static_cast<unsigned>(window_size.y * .8)}),
                              "Scratch Pad");
    // Code to load data here
}

Scratch_Pad::~Scratch_Pad() {
    // Code to store data here
    window.close();
}

// Must be put in the update loop
void Scratch_Pad::update() {
    window.clear();
    window.display();
}
