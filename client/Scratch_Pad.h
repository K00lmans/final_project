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
#include <fstream>

class Scratch_Pad {
    sf::RenderWindow window;
    std::string filepath; // The location of the specific file that this object saves and loads to
    std::vector<std::string> written_data; // The information the user has written in the scratch pad

public: // I'm actually using private member stuff, wooooooooo
    explicit Scratch_Pad(sf::Vector2u window_size, int player_num);

    ~Scratch_Pad();

    void update();
};


#endif //SCRATCH_PAD_H
