#include "Tile.hpp"
#include <iostream>
using namespace std;

Tile::Tile() : Tile(0, 0) {}

Tile::Tile(int new_x, int new_y) {
	x = new_x;
	y = new_y;
}

void Tile::setX(int new_x) {
	x = new_x;
}

void Tile::setY(int new_y) {
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