/*
 *Kodiak O'Masta, [TEAMMATES]
 *Created for CptS 122
 *Created 3-28-25
 *
 *An implementation of the game of Clue™ (or Cluedo™ if your weird)
 */

#ifndef LOCATION_H
#define LOCATION_H

#include <vector>

using std::vector;

class Location {
public:
    vector<Location> *neighbours;
    // When a player class exists, this would be a vector of players in the location
};


class Room : public Location {
public:
    // For when someone guesses someone else and they need to be moved
    void move_player_to_room();
};


#endif //LOCATION_H
