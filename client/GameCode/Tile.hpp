#pragma once
#include <iostream>
using namespace std;

class Tile {
private:
	int x;
	int y;

public:
	Tile();
	Tile(int new_x, int new_y);
	Tile(int coords[2]);

	int getX();
	int getY();

	void setX(int new_x);
	void setY(int new_y);

	bool operator==(const Tile& other) const;
	friend ostream& operator<<(ostream& os, const Tile& tile);
};

inline bool Tile::operator==(const Tile& other) const {
	return this->x == other.x && this->y == other.y;
}

inline int Tile::getX() {
	return x;
}

inline int Tile::getY() {
	return y;
}