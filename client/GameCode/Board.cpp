#include "Board.hpp"
#include <algorithm>
using namespace std;

void Board::placeToken(Token token, Tile tile) {
	int tile_x = tile.getX();
	int tile_y = tile.getY();

	if (token.isPlayer())
		game_board[tile_y][tile_x] &= ~PLAYER_MASK;
	else if (token.isWeapon())
		game_board[tile_y][tile_x] &= ~WEAPON_MASK;

	game_board[tile_y][tile_x] |= token.getID();
}

void Board::deleteToken(Token token, Tile tile) {
	int tile_x = tile.getX();
	int tile_y = tile.getY();

	int& tileData = game_board[tile_y][tile_x];

	// Extract player ID and weapon ID
	int characterID = tileData & 0xFF00; // Extract PP (high byte)
	int weaponID = tileData & 0x00FF; // Extract WW (low byte)

	// Check if the token exists in this tile before removing
	if (token.isPlayer() && characterID == token.getID()) {
		tileData &= ~0xFF00; // Remove player (clear PP)
	}
	else if (token.isWeapon() && weaponID == token.getID()) {
		tileData &= ~0x00FF; // Remove weapon (clear WW)
	}
}

void Board::displayBoard() {
	for (int i = 0; i < BOARD_HEIGHT; i++) {				// "i" is Y values
		for (int j = 0; j < BOARD_WIDTH; j++) {				// "j" is X values
			
			if (getValueAtTile(Tile(j, i)) != 0) {
				if (j != 0)
					cout << " ";
				DEBUG_printCharacterToken(getValueAtTile(Tile(j, i)));
				continue;
			}
			
			char charToPrint = '0';

			for (int k = 0; k < rooms.size(); k++) {
				if (rooms.at(k).isTileInRoom(Tile(j, i))) {
					charToPrint = rooms.at(k).getName().at(0);
				}
				vector<Tile> roomDoors = rooms.at(k).getDoors();

				/*
				vector<Tile> roomExits = rooms.at(k).getExits();
				
				if (find(roomExits.begin(), roomExits.end(), Tile(j, i)) != roomExits.end()) {
					charToPrint = 'X';
				}
				

				if (rooms.at(k).getChair() == Tile(j, i)) {
					charToPrint = 'H';
				}

				if (rooms.at(k).getTable() == Tile(j, i)) {
					charToPrint = 'T';
				}
				*/

				if (find(roomDoors.begin(), roomDoors.end(), Tile(j, i)) != roomDoors.end()) {
					charToPrint = 'E';
				}

			}

			if (j != 0)
				cout << " ";
			if(charToPrint == '0')
				cout << "\033[38;2;0;100;0m";
			if (charToPrint == 'U')
				cout << "\033[38;2;100;0;0m";
			if (charToPrint == 'E')
				cout << "\033[38;2;0;100;100m";
			cout << charToPrint;
			cout << "\033[0m";
		
		}
		cout << endl;
	}
}

void Board::addRoom(Room newRoom) {
	rooms.push_back(newRoom);
}

const vector<Room>& Board::getRooms() const
{
	return rooms;
}

vector<Tile> Board::getLegalMoves(Tile startTile) {
	for (int k = 0; k < rooms.size(); k++) {
		if (rooms.at(k).isTileInRoom(startTile)) {
			return rooms.at(k).getExits();
		}
	}

	int startX = startTile.getX();
	int startY = startTile.getY();

	vector<Tile> legalMoves = { Tile(startX - 1, startY), Tile(startX + 1, startY),
							   Tile(startX, startY - 1), Tile(startX, startY + 1) };

	for (int i = 0; i < legalMoves.size(); i++) {
		Tile tempTile = legalMoves.at(i);
		int tempX = tempTile.getX();
		int tempY = tempTile.getY();

		if (tempX >= BOARD_WIDTH || tempX < 0 || tempY >= BOARD_HEIGHT || tempY < 0) {
			legalMoves.erase(legalMoves.begin() + i);
			i--;
			continue;
		}

		for (int k = 0; k < rooms.size(); k++) {
			if (rooms.at(k).isTileInRoom(tempTile)) {
				vector<Tile> tempDoors = rooms.at(k).getDoors();
				if (find(tempDoors.begin(), tempDoors.end(), tempTile) == tempDoors.end()) {
					legalMoves.erase(legalMoves.begin() + i);
					i--;
					continue;
				}
			}
		}

		if (getValueAtTile(tempTile) != 0) {
			legalMoves.erase(legalMoves.begin() + i);
			i--;
			continue;
		}

	}
	return legalMoves;
}