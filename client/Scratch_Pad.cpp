/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-16-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "Scratch_Pad.h"

Scratch_Pad::Scratch_Pad(const sf::Vector2u window_size, const int player_num) {
    window = sf::RenderWindow(sf::VideoMode({window_size.x / 5, static_cast<unsigned>(window_size.y * .8)}),
                              "Scratch Pad", sf::Style::None);
    filepath = "scratch_pads/player_" + std::to_string(player_num) + ".txt";
    if (std::ifstream file(filepath); !file.is_open()) {
        // If there is an error opening the file, just lose the data
        for (int items = 0; items < 21; items++) {
            written_data.emplace_back("");
        }
    } else {
        std::string line;
        while (std::getline(file, line)) {
            written_data.push_back(line);
        }
        file.close();
        written_data.pop_back(); // Removes the extra newline because I can't be bothered to make a way not to add it
    }
}

Scratch_Pad::~Scratch_Pad() {
    std::ofstream file(filepath);
    for (const auto & i : written_data) {
        file << i << std::endl;
    }
    file.close();
    window.close();
}

// Must be put in the update loop
void Scratch_Pad::update() {
    while (const std::optional event = window.pollEvent()) {
    }
    window.clear(sf::Color(225, 198, 153)); // A pleasant shade of beige
    window.display();
}
