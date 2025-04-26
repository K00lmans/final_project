#pragma once
#include "Token.hpp"
#include "Tile.hpp"
#include "Board.hpp"

class Player {
	Token characterToken;
public:
	bool playing;
	Tile position;

	Player(const Token& new_characterToken, const Tile& new_position, bool new_playing);
	void movePlayer(Board& gameBoard);

	void setCharacterToken(const Token& new_characterToken);
	void setPosition(const Tile& new_position);

};