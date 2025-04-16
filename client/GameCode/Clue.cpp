// Clue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Token.hpp"
#include "TokenID.hpp"
#include "Board.hpp"
#include "Room.hpp"
#include "BoardSetup.hpp"
#include "Player.hpp"


using namespace std;
/*
int main()
{
    //cout << "Hello World!\n";

    Token Ms_Scarlet = Token(TokenID::MS_SCARLET);
    Token Col_Mustard = Token(TokenID::COL_MUSTARD);
    Token Prof_Plum = Token(TokenID::PROF_PLUM);
    Token Mr_Green = Token(TokenID::MR_GREEN);
    Token Mrs_Peacock = Token(TokenID::MRS_PEACOCK);
    Token Mrs_White = Token(TokenID::MRS_WHITE);

    //Board clue_board = Board();
    Board clue_board = getBoardFromFile();

    

    Player player1 = Player(Mrs_Peacock, Tile(0, 18));

    int* board_visualization = clue_board.getBoard();

    cout << endl << endl; // REMOVE LATER!!

    clue_board.deleteToken(Mrs_Peacock, Tile(0, 18));
    player1.setPosition(Tile(6, 15));
    clue_board.placeToken(Mrs_Peacock, Tile(6, 15));

    while (1) {
        clue_board.displayBoard();
        player1.movePlayer(clue_board);
        system("cls");
    }
}
*/