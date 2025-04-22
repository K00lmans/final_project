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
    // Would it be better to have it be a vector of sf::Text, yes, but by this point too much had been written with it
    // as a string and I have deluded myself into thinking there is probably some benefit

    sf::Font font = sf::Font("client/graphics/NotoSans-Black.ttf");
    unsigned int text_size = 1;
    // The text on the left side of the scratch pad that indicates items, rooms, and people
    sf::Text left_side_text = sf::Text(font, "Colonel Mustard\nMiss Scarlet\nProfessor Plum\nMr. Green\n"
                                             "Mrs. White\nMrs. Peacock\nRope\nLead Pipe\nKnife\nWrench\nCandlestick\n"
                                             "Revolver\nBottle of Poison\nStudy\nHall\nLounge\nDining Room\nKitchen\n"
                                             "Ballroom\nConservatory\nBilliard Room\nLibrary", text_size);

    // The size of one text square. Its width is half the size of the screen and the height based on how many lines of
    // text. Used as an alignment indicator for a whole bunch of things as well as setting up the grid
    sf::RectangleShape base_box;
    std::vector<sf::RectangleShape> grid;

    int selected_box = -1;

    void draw_grid();

    void draw_text();

public: // I'm actually using private member stuff, wooooooooo
    explicit Scratch_Pad(sf::Vector2u window_size, int player_num);

    ~Scratch_Pad();

    void update();
};


#endif //SCRATCH_PAD_H
