#pragma once
#include <iostream>
using namespace std;

class Tile {
	int x;
	int y;

public:
	Tile();
	Tile(int new_x, int new_y);

	explicit Tile(int coords[2]);

	int getX() const;
	int getY() const;

	void setX(int new_x);
	void setY(int new_y);

	bool operator==(const Tile& other) const;
	bool operator!=(const Tile& other) const;
	friend ostream& operator<<(ostream& os, const Tile& tile);
};

inline bool Tile::operator==(const Tile& other) const {
	return this->x == other.x && this->y == other.y;
}

inline bool Tile::operator!=(const Tile& other) const {
	return this->x != other.x || this->y != other.y;
}

inline int Tile::getX() const {
	return x;
}

inline int Tile::getY() const {
	return y;
}
