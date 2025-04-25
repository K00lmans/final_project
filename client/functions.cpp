/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 4-23-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "main.h"

unsigned int get_window_x_size() {
    return static_cast<unsigned int>(screen_size.y * window_scaler * (background_size[0] / background_size[1]));
}

void pick_characters(Player* player_list[6]) {
    auto selection_window = sf::RenderWindow(sf::VideoMode({screen_size.x / 2, screen_size.y / 5}),
        "Character Selection", sf::Style::Close);
    const auto selection_boxes = create_buttons(create_standard_box());

    bool finished = false;
    while (selection_window.isOpen()) {
        while (const std::optional event = selection_window.pollEvent()) {
            if (event->is<sf::Event::Closed>() || finished) {
                selection_window.close();
            } else if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                for (int button = 0; button < 6; button++) {
                    if (player_list[button] == nullptr && selection_boxes[button].second.getGlobalBounds().contains(
                        static_cast<sf::Vector2<float>>(mouse->position))) {
                        player_list[button] = new Player(tokens[button], starting_positions[button]);
                    }
                }
                if (selection_boxes[6].second.getGlobalBounds().contains(static_cast<sf::Vector2<float>>(mouse->position))) {
                    selection_window.close();
                }
            }
        }

        selection_window.clear(sf::Color(128, 128, 128));
        finished = true;
        for (int button = 0; button < 7; button++) {
            if (button != 6) { // Prevents errors with over-reading the array
                if (player_list[button] == nullptr) {
                    selection_window.draw(selection_boxes[button].second);
                    selection_window.draw(selection_boxes[button].first);
                    finished = false;
                }
            } else { // Drawing the close button
                selection_window.draw(selection_boxes[button].second);
                selection_window.draw(selection_boxes[button].first);
            }
        }
        selection_window.display();
    }
}

sf::RectangleShape* create_standard_box() {
    const auto standard = new sf::RectangleShape({static_cast<float>(screen_size.x / 12.0), // 1/6th the window width
        static_cast<float>(screen_size.y / 10.0)}); // 1/2 the window height
    standard->setOutlineThickness(screen_size.x / 360.0); // Should be three pixels on a 1080p screen
    standard->setOutlineColor(sf::Color::Black);
    standard->setFillColor(sf::Color(175, 175, 175));
    return standard;
}

std::vector<std::pair<sf::Text, sf::RectangleShape>> create_buttons(const sf::RectangleShape *standard) {
    const std::vector button_text = {"Colonel Mustard", "Miss Scarlet", "Professor Plum", "Mr. Green", "Mrs. White",
        "Mrs. Peacock", "Done"};
    std::vector<std::pair<sf::Text, sf::RectangleShape>> buttons;
    for (int box = 0; box < 7; box++) { // This mess makes the selection boxes
        int text_size = 1;
        buttons.emplace_back(sf::Text(font, button_text[box], text_size), sf::RectangleShape(*standard));
        buttons[box].second.setPosition({static_cast<float>(box) * standard->getSize().x, 0});
        while (buttons[box].first.getGlobalBounds().size.x < buttons[box].second.getSize().x -
               buttons[box].second.getOutlineThickness()) {
            text_size++;
            buttons[box].first.setCharacterSize(text_size);
        }
        text_size--;
        buttons[box].first.setCharacterSize(text_size);
        buttons[box].first.setPosition({buttons[box].second.getPosition().x,
            static_cast<float>(standard->getSize().y / 2 - buttons[box].first.getCharacterSize() / 1.5)});
    }
    buttons[6].second.setPosition({static_cast<float>(screen_size.x / 4.0 - // Half the window
        standard->getSize().x / 2), standard->getSize().y + standard->getOutlineThickness()});
    buttons[6].first.setPosition({static_cast<float>(screen_size.x / 4.0 -
        standard->getSize().x / 2), static_cast<float>(standard->getSize().y * 1.5 -
            buttons[6].first.getCharacterSize() / 1.5)});

    delete standard; // Should probably be a smart pointer, but I'm not that smart so no smart pointer
    return buttons;
}
