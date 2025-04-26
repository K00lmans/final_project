/*
 *Athena Boose, Kodiak O'Masta, Ryan Gradinaru
 *Created for CptS 122
 *Created 3-28-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 *
 *This program is dedicated to my (Kodiak's) dad, who in 1981 was working towards a degree in computer science. The
 *final project for the class he was taking at the time was to program a game of Clue™. Of course, since it was "Ye
 *Olden Times" he had no fancy languages to do it in, and instead was required to do it entirely in assembly (the
 *horror!). He was able to finish the project, but left with some less than positive feelings about the game. Of course,
 *that would not be the end of it, as his (now) ex-wife was also working towards the same degree and was a year behind
 *him. Now, she was not nearly as good as my dad and so after a full semester of him helper her, she somehow convinced
 *him to do the final project for her. He now, once again, had to program Clue™ in assembly, except this time he had to
 *do it in a completely different way to avoid plagiarizing himself. The sum total of these experiences had left him
 *with a deep disdain for Clue™ that has lasted to this day, and has become a running gag in my family. As such, I felt
 *that there could be no more appropriate choice for what to code for this final project, and as such this code is
 *dedicated to his struggles with this game.
 */

#include "main.h"

int main() {
    screen_size = sf::VideoMode::getDesktopMode().size;

    // Game setup
    Board clue_board = getBoardFromFile(); // Can this not just be a constructor? -Kodiak
    Player *players[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; // I have an irrational love of c arrays
    pick_characters(players);
    int current_player = generate_random_int(0, 5); // Picks the starting player

    // Window setup
    auto main_game_window = sf::RenderWindow(sf::VideoMode({get_window_x_size(),
        static_cast<unsigned>(screen_size.y * window_scaler)}), "Clue", sf::Style::Close);
    auto background_image = sf::Texture("client/graphics/clue_board.jpg");
    background_image.setSmooth(true); // Should be fine since it is the background
    auto background = sf::Sprite(background_image);
    background.setScale({
        static_cast<float>(get_window_x_size() / background_size[0]),
        static_cast<float>(screen_size.y * window_scaler / background_size[1])
    });
    Scratch_Pad::clear_data(); // Emptys old data
    auto current_users_pad = std::make_unique<Scratch_Pad>(screen_size, current_player + 1);

    // Main game loop
    while (main_game_window.isOpen()) {
        while (const std::optional event = main_game_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                current_users_pad.reset();
                main_game_window.close();
            }
        }

        main_game_window.clear();
        main_game_window.draw(background);
        main_game_window.display();
        if (current_users_pad != nullptr) {
            current_users_pad->update();
        }
    }

    for (auto &player : players) {
        delete player;
    }
    return 0;
}
