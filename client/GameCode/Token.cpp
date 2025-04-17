#include "Token.hpp"

Token::Token(TokenID new_id) {
	setID(new_id);
}

Token::Token(const Token& tok_copy)
{
	id = tok_copy.id;
}

void Token::setID(TokenID new_id) {
	id = (int)new_id;
}

bool Token::isPlayer() {
	int id = getID();
	return (id >= 0x1100 && id < 0xff00);
}

bool Token::isWeapon() {
	int id = getID();
	return (id >= 0x11 && id < 0xff);
}