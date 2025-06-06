#include "Tile.hpp"
#include <iostream>
using namespace std;

Tile::Tile() : Tile(0, 0) {}

Tile::Tile(const int new_x, const int new_y) {
	x = new_x;
	y = new_y;
}

void Tile::setX(const int new_x) {
	x = new_x;
}

void Tile::setY(const int new_y) {
	y = new_y;
}

Tile::Tile(int coords[2]) {
	setX(coords[0]);
	setY(coords[1]);
}

ostream& operator<<(ostream& os, const Tile& tile) {
	os << "Tile(" << tile.x << ", " << tile.y << ")";
	return os;
}