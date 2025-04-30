#include "Player.hpp"
#include "Board.hpp"
#include <iostream>
#include "Token.hpp"
#include "Room.hpp"

using namespace std;

void Player::setCharacterToken(const Token& new_characterToken) {
	characterToken = new_characterToken;
}
void Player::setPosition(const Tile& new_position) {
	position = new_position;
}

Player::Player(const Token& new_characterToken, const Tile& new_position, const bool new_playing)
	: characterToken(new_characterToken), position(new_position), playing(new_playing) {}

void Player::movePlayer(Board& gameBoard) {
	const vector<Tile> legalMoves = gameBoard.getLegalMoves(position);
	
	cout << "Choose a move:" << endl;
	for (int i = 0; i < legalMoves.size(); i++) {
		 cout << i + 1 << ": " << legalMoves.at(i) << endl;
	}

	int choice;
	do {
		cout << endl << "Where do you wish to go (enter a number from " << 1 << " to "
			<< legalMoves.size() << " inclusive): ";

		cin >> choice;

		if (choice < 1 || choice > legalMoves.size()) {
			cout << choice << " is out of bounds!!" << endl;
		}
	} while (choice < 1 || choice > legalMoves.size());

	gameBoard.deleteToken(characterToken, position);
	position = legalMoves.at(choice - 1);
	

	for (const Room& boardRoom : gameBoard.getRooms()) {
		for (Tile door : boardRoom.getDoors()) {
			if (position == door) {
				position = boardRoom.getChair();
			}
		}
	}

	gameBoard.placeToken(characterToken, position);
}