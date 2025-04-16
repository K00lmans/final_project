/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-16-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 *
 *A container for the window relating to the scratch pad
 */

#ifndef SCRATCH_PAD_H
#define SCRATCH_PAD_H

#include <SFML/Graphics.hpp>

class Scratch_Pad {
    sf::RenderWindow window;

    explicit Scratch_Pad(sf::Vector2u window_size);

    ~Scratch_Pad();

public: // I'm actually using private member stuff, wooooooooo
    void update();
};


#endif //SCRATCH_PAD_H
