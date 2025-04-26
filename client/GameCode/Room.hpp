#pragma once
#include "Tile.hpp"
#include <vector>
#include <array>
#include <string>
using namespace std;

class Room {
	string name;


	vector<array<Tile, 4>> room_bounds; //Each room will have "sections" each section will be a different bounding box.
										//This is because the clue board is wonky.
	vector<Tile> doors;
	vector<Tile> exits;

	Tile chair;
	Tile table;
public:
	Room() = delete;
	explicit Room(const string &new_name);

	void addBoundsRaw(const Tile& top_left, const Tile& top_right, const Tile& bottom_left, const Tile& bottom_right);
	void addBoundsArray(const array<Tile, 4>& newBounds);

	void setDoors(const vector<Tile>& new_doors);
	void setExits(const vector<Tile>& new_doors);

	void setChair(const Tile& new_chair);
	void setTable(const Tile& new_table);

	[[nodiscard]] const string& getName() const;

	[[nodiscard]] const vector<array<Tile, 4>>& getBounds() const;

	[[nodiscard]] const vector<Tile>& getDoors() const;
	[[nodiscard]] const vector<Tile>& getExits() const;

	[[nodiscard]] const Tile& getChair() const;
	[[nodiscard]] const Tile& getTable() const;


	[[nodiscard]] bool isTileInRoom(Tile tile) const;
};

inline const string& Room::getName() const {
	return name;
}

inline const vector<array<Tile, 4>>& Room::getBounds() const {
	return room_bounds; // Read-only access
}

inline const vector<Tile>& Room::getDoors() const {
	return doors;
}
inline const vector<Tile>& Room::getExits() const {
	return exits;
}

inline const Tile& Room::getChair() const {
	return chair;
}
inline const Tile& Room::getTable() const {
	return table;
}