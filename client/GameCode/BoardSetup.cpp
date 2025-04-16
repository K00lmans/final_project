#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "Room.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include "TokenID.hpp"
#include "Token.hpp"
#include "BoardData.hpp"

using namespace std;


//Board file will be in format:
//ROOM
//RoomName Chairx, Chairy, Tablex, Tabley
//Border1 x, Border1 y, Border2 x, Border2 y, Border3 x, Border3 y, Border4 x, Border4 y
//...
//END BORDER
//Num doors, Door1, ...
//num exits, Exit1, ...
//ROOM
//...
//CHARACTER
//Character Name, Starting Tile
//...
//ROOM
//UNREACHABLE, -1, -1, -1, -1
//outside borders and middle "room"
//END BORDER
//0
//0

string getNextFragment(istream& boardFile) {
	string fragment;
	char ch;

	while (boardFile.get(ch)) {
		if (ch == ',' || ch == '\n') {
			break; 
		}
		fragment += ch; 
	}

	return fragment;
}

Tile getTileFromStream(istream& boardFile) {
	string fragmentString;
	int tileX;
	int tileY;

	fragmentString = getNextFragment(boardFile);
	tileX = stoi(fragmentString);		 //get chair x
	fragmentString = getNextFragment(boardFile);
	tileY = stoi(fragmentString);		 //get chair y

	return Tile(tileX, tileY);
}

vector<Tile> getTileVectorFromStream(istream& boardFile) {
	string fragmentString;
	vector<Tile> newVector;
	int vectorLength;

	fragmentString = getNextFragment(boardFile);
	vectorLength = stoi(fragmentString);

	for (int i = 0; i < vectorLength; i++) {
		newVector.push_back(getTileFromStream(boardFile));
	}
	return newVector;
}

array<Tile, 4> getBorderFromString(string& borderLine) {
	array<Tile, 4> newBorder;
	stringstream str_stream(borderLine);

	for (int i = 0; i < 4; i++)
		newBorder[i] = getTileFromStream(str_stream);

	return newBorder;
}

Room getRoomFromStream(istream& boardFile) {
	string dataLine;
	string fragmentString;

	getline(boardFile, fragmentString, ',');
	Room newRoom = Room(fragmentString); //get room name

	newRoom.setChair(getTileFromStream(boardFile));
	newRoom.setTable(getTileFromStream(boardFile));

	getline(boardFile, dataLine);

	while (!(dataLine.compare("END BORDERS") == 0)) {
		array<Tile, 4> newBorder = getBorderFromString(dataLine);
		newRoom.addBoundsArray(newBorder);
		getline(boardFile, dataLine);
	}

	newRoom.setDoors(getTileVectorFromStream(boardFile));
	newRoom.setExits(getTileVectorFromStream(boardFile));

	//cout << fragmentString << endl;

	return newRoom;
}

void addCharacterFromStream(istream& boardFile, Board& newBoard) {
	string dataLine;
	string fragmentString;

	getline(boardFile, fragmentString, ',');
	int tokenID = getTokenID(fragmentString);
	

	newBoard.placeToken(Token((TokenID)tokenID), getTileFromStream(boardFile));
}

Board getBoardFromFile() { // THIS IS MEANT TO READ THE FILE INTO A BOARD OBJECT!! REFACTOR!!
	string dataLine;
	stringstream boardFile(board_string);

	Board newBoard =  Board();

	

	if (boardFile.fail()) {
		return newBoard;
	}

	while (getline(boardFile, dataLine)) {
		//cout << dataLine << endl; //GET TAG - EITHER "CHARACTER" *OR* "ROOM"
		if (dataLine.compare("ROOM") == 0) {
			newBoard.addRoom(getRoomFromStream(boardFile));
			continue;
		}

		if (dataLine.compare("CHARACTER") == 0) {
			addCharacterFromStream(boardFile, newBoard);
			continue;
		}

	}
	return newBoard;
}