/*
 *Kodiak O'Masta, [TEAMMATES]
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

#include <SFML/Graphics.hpp>
#include "Scratch_Pad.h"

int main() {
    const auto screen_size = sf::VideoMode::getDesktopMode().size;
    auto main_game_window = sf::RenderWindow(sf::VideoMode({screen_size.x / 2, screen_size.y / 2}), "Clue");
    std::unique_ptr<Scratch_Pad> current_users_pad;

    while (main_game_window.isOpen()) {
        while (const std::optional event = main_game_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                main_game_window.close();
            }
        }

        main_game_window.clear();
        main_game_window.display();
        if (current_users_pad != nullptr) {
            current_users_pad->update();
        }
    }
}
