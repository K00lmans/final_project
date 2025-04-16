#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

enum class TokenID {
	MS_SCARLET = 0x1100,
	COL_MUSTARD = 0x1200,
	PROF_PLUM = 0x1300,
	MR_GREEN = 0x1400,
	MRS_PEACOCK = 0x1500,
	MRS_WHITE = 0x1600,

	CANDLESTICK = 0x21,
	KNIFE = 0x22,
	LEAD_PIPE = 0x23,
	ROPE = 0x24,
	WRENCH = 0x25
};


string getTokenString(int token_id);
int getTokenID(const std::string& characterName);
void DEBUG_printCharacterToken(int token_id);
