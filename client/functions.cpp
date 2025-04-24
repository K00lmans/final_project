/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 4-23-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "main.h"

unsigned int get_window_x_size() {
    return static_cast<unsigned int>(((screen_size.y * window_scaler) * (background_size[0] / background_size[1])));
}

void pick_characters(Player* player_list[6]) {
    std::vector<std::pair<sf::Text, sf::RectangleShape>> selection_boxes;
    const std::vector character_names = {"Colonel Mustard", "Miss Scarlet", "Professor Plum", "Mr. Green", "Mrs. White",
        "Mrs. Peacock"};

    auto selection_window = sf::RenderWindow(sf::VideoMode({screen_size.x / 4, screen_size.y / 4}),
        "Character Selection", sf::Style::Close);
    auto standard = create_standard_box();
    for (int box = 0; box < 6; box++) { // This mess makes the selection boxes
        selection_boxes.emplace_back(sf::Text(font, character_names[box], 1), sf::RectangleShape(standard));
        selection_boxes[box].second.setPosition({static_cast<float>(box) * standard.getSize().x, 0});
        selection_boxes[box].first.setPosition({(static_cast<float>(box) * standard.getSize().x) +
            standard.getOutlineThickness(), standard.getOutlineThickness()});
        int text_size;
        for (text_size = 2; selection_boxes[box].first.getGlobalBounds().size.x <= (standard.getSize().x -
            (standard.getOutlineThickness() * 2)) && selection_boxes[box].first.getGlobalBounds().size.y <=
            (standard.getSize().y - (standard.getOutlineThickness() * 2)); text_size++) { // What a loop
            selection_boxes[box].first.setCharacterSize(text_size);
        }
        text_size--;
        selection_boxes[box].first.setCharacterSize(text_size);
    }

    while (selection_window.isOpen()) {
        while (const std::optional event = selection_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                selection_window.close();
            }
        }

        selection_window.clear(sf::Color(128, 128, 128));
        selection_window.display();
    }
}

sf::RectangleShape create_standard_box() {
    sf::RectangleShape standard({static_cast<float>(screen_size.x / 24.0), // 1/6th the window width
        static_cast<float>(screen_size.y / 8.0)}); // 1/2 the window height
    standard.setOutlineThickness(screen_size.x / 360.0); // Should be three pixels on a 1080p screen
    standard.setOutlineColor(sf::Color::Black);
    return standard;
}
