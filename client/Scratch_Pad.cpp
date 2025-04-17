/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-16-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "Scratch_Pad.h"

Scratch_Pad::Scratch_Pad(sf::Vector2u window_size, const int player_num) {
    window = sf::RenderWindow(sf::VideoMode({window_size.x / 4, static_cast<unsigned>(window_size.y * .8)}),
                              "Scratch Pad", sf::Style::None);
    filepath = "scratch_pads/player_" + std::to_string(player_num) + ".txt";
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // If there is an error opening the file, just don't. This will cause an error and crash the program.
        // This is good. I swear.
        delete this;
    } else {
        std::string line;
        while (std::getline(file, line)) {
            written_data.push_back(line);
        }
        file.close();
    }
}

Scratch_Pad::~Scratch_Pad() {
    // Code to store data here
    window.close();
}

// Must be put in the update loop
void Scratch_Pad::update() {
    while (const std::optional event = window.pollEvent()) {
    }
    window.clear();
    window.display();
}
