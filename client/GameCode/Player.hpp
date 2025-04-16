#include "Token.hpp"
#include "Tile.hpp"
#include "Board.hpp"

class Player {
private:
	Token characterToken;
	Tile position;
public:
	Player(const Token& new_characterToken, const Tile& new_position);
	void movePlayer(Board& gameBoard);

	void setCharacterToken(const Token& new_characterToken);
	void setPosition(const Tile& new_position);

};