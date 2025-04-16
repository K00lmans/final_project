#pragma once
#include "TokenID.hpp"

int constexpr PLAYER_MASK = 0xff00;
int constexpr WEAPON_MASK = 0x00ff;

class Token {
private:
	int id;

public:
	Token() = delete;
	Token(TokenID new_id);
	Token(const Token& new_id);

	int getID() const;
	void setID(TokenID new_id);

	bool isPlayer();
	bool isWeapon();
};

inline int Token::getID() const {
	return (int)id;
}