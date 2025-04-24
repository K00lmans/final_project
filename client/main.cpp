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
    // Window setup
    screen_size = sf::VideoMode::getDesktopMode().size;
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
    std::unique_ptr<Scratch_Pad> current_users_pad;

    // Game setup
    auto Ms_Scarlet = Token(TokenID::MS_SCARLET);
    auto Col_Mustard = Token(TokenID::COL_MUSTARD);
    auto Prof_Plum = Token(TokenID::PROF_PLUM);
    auto Mr_Green = Token(TokenID::MR_GREEN);
    auto Mrs_Peacock = Token(TokenID::MRS_PEACOCK);
    auto Mrs_White = Token(TokenID::MRS_WHITE);
    Board clue_board = getBoardFromFile(); // Can this not just be a constructor? -Kodiak
    Player *players[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; // I have an irrational love of c arrays

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

    Player player1 = Player(Mrs_Peacock, Tile(0, 18));

    clue_board.deleteToken(Mrs_Peacock, Tile(0, 18));
    player1.setPosition(Tile(6, 15));
    clue_board.placeToken(Mrs_Peacock, Tile(6, 15));

    for (auto &player : players) {
        delete player;
    }
    return 0;
}
