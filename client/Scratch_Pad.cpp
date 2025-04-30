/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 4-16-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#include "Scratch_Pad.h"

double lines_on_left_size = 22;

Scratch_Pad::Scratch_Pad(const sf::Vector2u window_size, const int player_num) {
    // Creation of the window
    window = sf::RenderWindow(sf::VideoMode({window_size.x / 4, static_cast<unsigned>(window_size.y * .8)}),
                              "Scratch Pad", sf::Style::Titlebar);
    window.setPosition({0, 0}); // Moves out of the way of the main window

    // Loading of the data
    filepath = "client/scratch_pads/player_" + std::to_string(player_num) + ".txt";
    if (std::ifstream file(filepath); !file.is_open()) {
        // If there is an error opening the file, just lose the data
        for (int items = 0; items < 22; items++) {
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
        if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            for (auto &box: grid) {
                if (box.getOutlineColor() == sf::Color::Red) {
                    box.setOutlineColor(sf::Color(128, 128, 128));
                }
            }
            if (mouse->position.x > base_box.getSize().x) {
                selected_box = static_cast<int>(mouse->position.y / base_box.getSize().y);
                grid[selected_box + static_cast<int>(lines_on_left_size)].setOutlineColor(sf::Color::Red);
            } else {
                selected_box = -1;

            }
        } else if (const auto *key = event->getIf<sf::Event::TextEntered>()) {
            if (selected_box != -1) {
                if (key->unicode < 128) {
                    char c = key->unicode;
                    if (c == '\b') {
                        if (!written_data[selected_box].empty()) {
                            written_data[selected_box].pop_back();
                        }
                    } else {
                        written_data[selected_box].push_back(c);
                    }
                }
            }
        }
    }
    window.clear(sf::Color(225, 198, 153)); // A pleasant shade of beige
    draw_grid();
    draw_text();
    window.display();
}

void Scratch_Pad::draw_grid() {
    for (const auto &box: grid) {
        window.draw(box);
    }
}

void Scratch_Pad::draw_text() {
    window.draw(left_side_text); // Moved here for clarity with the function name

    sf::Text text(font, "", text_size);
    std::string combined_written_data;
    for (const auto &i: written_data) {
        combined_written_data += i;
        combined_written_data += '\n';
    }
    text.setString(combined_written_data);
    text.setPosition({base_box.getSize().x, 0}); // Aligns this text to the right
    window.draw(text);
}

void Scratch_Pad::clear_object() {
    for (auto &i: written_data) {
        i.clear();
    }
}

void Scratch_Pad::clear_data() {
    // This is unnecessarily extendable and I think that's funny
    for (const auto &pad: std::filesystem::directory_iterator("client/scratch_pads")) {
        std::ofstream file(pad.path());
        for (int lines = 0; lines < lines_on_left_size; lines++) {
            file << std::endl;
        }
        file.close();
    }
}
