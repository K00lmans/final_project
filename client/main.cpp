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
#include <iostream>
#include "GameCode/Token.hpp"
#include "GameCode/TokenID.hpp"
#include "GameCode/Board.hpp"
#include "GameCode/Room.hpp"
#include "GameCode/BoardSetup.hpp"
#include "GameCode/Player.hpp"


[[noreturn]] int main() {
    /*
    const auto screen_size = sf::VideoMode::getDesktopMode().size;
    auto main_game_window = sf::RenderWindow(sf::VideoMode({screen_size.x / 2, screen_size.y / 2}), "Clue",
                                             sf::Style::Close);
    Scratch_Pad *current_users_pad = nullptr;

    while (main_game_window.isOpen()) { // Main game loop
        while (const std::optional event = main_game_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                main_game_window.close();
            }
        }

        main_game_window.clear();

        // graphics go HERE!!
        

        main_game_window.display();
        if (current_users_pad != nullptr) {
            current_users_pad->update();
        }
    }
    */

    auto Ms_Scarlet = Token(TokenID::MS_SCARLET);
    auto Col_Mustard = Token(TokenID::COL_MUSTARD);
    auto Prof_Plum = Token(TokenID::PROF_PLUM);
    auto Mr_Green = Token(TokenID::MR_GREEN);
    auto Mrs_Peacock = Token(TokenID::MRS_PEACOCK);
    auto Mrs_White = Token(TokenID::MRS_WHITE);

    //Board clue_board = Board();
    Board clue_board = getBoardFromFile();



    Player player1 = Player(Mrs_Peacock, Tile(0, 18));

    int* board_visualization = clue_board.getBoard();

    cout << endl << endl; // REMOVE LATER!!

    clue_board.deleteToken(Mrs_Peacock, Tile(0, 18));
    player1.setPosition(Tile(6, 15));
    clue_board.placeToken(Mrs_Peacock, Tile(6, 15));

    while (true) {
        clue_board.displayBoard();
        player1.movePlayer(clue_board);
        system("cls");
    }

    return 0;
}
