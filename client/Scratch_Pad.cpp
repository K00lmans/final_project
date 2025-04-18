/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-16-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "Scratch_Pad.h"

double lines_on_left_size = 22;

// This needs lots of testing
Scratch_Pad::Scratch_Pad(const sf::Vector2u window_size, const int player_num) {
    // Creation of the window
    window = sf::RenderWindow(sf::VideoMode({window_size.x / 4, static_cast<unsigned>(window_size.y * .8)}),
                              "Scratch Pad", sf::Style::Titlebar);

    // Loading of the data
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
    }

    // Setting up the left side text
    // This bit of code makes it so that the text size is only big enough to hit half of the window
    while (left_side_text.getGlobalBounds().size.x < (window_size.x) / 8.0) {
        text_size++;
        left_side_text.setCharacterSize(text_size);
    }
    text_size--;
    left_side_text.setCharacterSize(text_size);
    // All this code makes it so the bottem of the window is aligned with the bottem of the text
    auto view = window.getView();
    window.setSize({window.getSize().x, static_cast<unsigned>(left_side_text.getGlobalBounds().size.y)});
    view.setSize({
        static_cast<float>(window.getSize().x),                   // For spacing ↓
        static_cast<float>(left_side_text.getGlobalBounds().size.y + text_size / 3.0)
    });
    view.setCenter({view.getSize().x / 2, view.getSize().y / 2});
    window.setView(view);

    // Setting up the grid lines
    base_box.setSize({view.getSize().x / 2, static_cast<float>(view.getSize().y / lines_on_left_size)});
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < lines_on_left_size; y++) {
            auto new_box = sf::RectangleShape(base_box);
            new_box.setPosition({base_box.getSize().x * static_cast<float>(x), base_box.getSize().y * static_cast<float>(y)});
            new_box.setFillColor(sf::Color::Transparent);
            new_box.setOutlineColor(sf::Color(128, 128, 128)); // A legible shade of GRÆY
            new_box.setOutlineThickness(1);
            grid.push_back(new_box);
        }
    }
}

Scratch_Pad::~Scratch_Pad() {
    std::ofstream file(filepath);
    for (const auto &i: written_data) {
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
    window.draw(left_side_text);
    draw_grid();
    window.display();
}

void Scratch_Pad::draw_grid() {
    for (const auto &box: grid) {
        window.draw(box);
    }
}

void Scratch_Pad::draw_text() {
}
