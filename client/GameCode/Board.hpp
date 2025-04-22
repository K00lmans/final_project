#pragma once
#include "Tile.hpp"
#include "Room.hpp"
#include "Token.hpp"

#define BOARD_WIDTH 24
#define BOARD_HEIGHT 25

class Board {
	/* CLUE BOARD HAS A BORDER (signified by x)
	*   1 2 3 4 5
	* 1 x x x x x
	* 2 x 0 0 0 x
	* 3 x 0 0 T x
	* 4 x 0 0 0 x
	* 5 x x x x x
	* uses column row notation
	* 
	* t is at Tile(4, 3), which translates to game_board[3][4]
	*/

	int game_board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };
	vector<Room> rooms;

public:
	int* getBoard();
	int getValueAtTile(Tile tile) const;

	void addRoom(Room newRoom);

	const vector<Room>& getRooms() const;

	void placeToken(Token token, Tile tile);
	void deleteToken(Token token, Tile tile);
	void displayBoard();

	vector<Tile> getLegalMoves(Tile startTile);
};

inline int* Board::getBoard() {
	return &game_board[0][0];
}

inline int Board::getValueAtTile(Tile tile) const {
	return game_board[tile.getY()][tile.getX()];
}
