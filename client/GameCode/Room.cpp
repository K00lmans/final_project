#include "Room.hpp"

Room::Room(string new_name) {
	name = new_name;
	//Room::addBounds(Tile(), Tile(), Tile(), Tile());

	setDoors(vector<Tile>());
	setExits(vector<Tile>());

	chair = Tile(0, 0);
	table = Tile(0, 0);
}

void Room::addBoundsRaw(const Tile& top_left, const Tile& top_right, const Tile& bottom_left, const Tile& bottom_right) {
	array<Tile, 4> new_bounds;
	new_bounds[0] = Tile(top_left);
	new_bounds[1] = Tile(top_right);
	new_bounds[2] = Tile(bottom_left);
	new_bounds[3] = Tile(bottom_right);

	room_bounds.push_back(new_bounds);
}

void Room::addBoundsArray(const array<Tile, 4>& newBounds) {
	room_bounds.push_back(newBounds);
}

void Room::setDoors(const vector<Tile>& new_doors) {
	doors = new_doors;
}

void Room::setExits(const vector<Tile>& new_exits) {
	exits = new_exits;
}

void Room::setChair(const Tile& new_chair) {
	chair = Tile(new_chair);
}

void Room::setTable(const Tile& new_chair) {
	table = Tile(new_chair);
}

/**
 * WARNING: Does not work if room is not rectangular
 * @brief Finds if a tile value should be flagged as being within a room
 *
 * @param 1.) Tile
 * @return Whether or not the tile is in the room
 */
bool Room::isTileInRoom(Tile tile) const {
	auto bounds_iter = room_bounds.begin();

	while (bounds_iter != room_bounds.end()) {
		int min_index = 0;
		int max_index = 0;

		array<Tile, 4> subspace = *bounds_iter;

		for (int i = 0; i < 4; i++) {
			min_index = (subspace[i].getX() <= subspace[min_index].getX() &&
				subspace[i].getY() <= subspace[min_index].getY()) ?
				i : min_index;
		}

		for (int i = 0; i < 4; i++) {
			max_index = (subspace[i].getX() >= subspace[max_index].getX() &&
				subspace[i].getY() >= subspace[max_index].getY()) ?
				i : max_index;
		}

		if ((subspace[min_index].getX() <= tile.getX() && subspace[max_index].getX() >= tile.getX()) &&
			(subspace[min_index].getY() <= tile.getY() && subspace[max_index].getY() >= tile.getY())) {
			return true;
		}

		//cout << max_index * 10 + min_index;
		bounds_iter++;
	}
	return false;
}