#include "TokenID.hpp"

static const unordered_map<int, std::string> token_lookup_table = {
		{static_cast<int>(TokenID::MS_SCARLET), "Ms. Scarlet"},
		{static_cast<int>(TokenID::COL_MUSTARD), "Colonel Mustard"},
		{static_cast<int>(TokenID::PROF_PLUM), "Professor Plum"},
		{static_cast<int>(TokenID::MR_GREEN), "Mr. Green"},
		{static_cast<int>(TokenID::MRS_PEACOCK), "Mrs. Peacock"},
		{static_cast<int>(TokenID::MRS_WHITE), "Mrs. White"},

		{static_cast<int>(TokenID::CANDLESTICK), "Candlestick"},
		{static_cast<int>(TokenID::KNIFE), "Knife"},
		{static_cast<int>(TokenID::LEAD_PIPE), "Lead pipe"},
		{static_cast<int>(TokenID::ROPE), "Rope"},
		{static_cast<int>(TokenID::WRENCH), "Wrench"}
};

/**
 * WARNING: Does not work if tokens are overlapped. PRIORITY: LOW
 */
string getTokenString(int token_id) {
	auto table_iterator = token_lookup_table.find(token_id);
	return (table_iterator == token_lookup_table.end()) ? "none" : table_iterator->second;
}

int getTokenID(const std::string& characterName) {
	for (const auto& [id, token_name] : token_lookup_table) {
		if (token_name == characterName) {
			return id; 
		}
	}
	return -1; 
}

void DEBUG_printCharacterToken(int token_id) {
	switch (token_id) {
	case 0:
		break;
	case static_cast<int>(TokenID::MS_SCARLET):
		cout << "\033[38;2;255;36;0mS\033[0m";
		break;
	case static_cast<int>(TokenID::COL_MUSTARD):
		cout << "\033[38;2;255;219;88mM\033[0m";
		break;
	case static_cast<int>(TokenID::PROF_PLUM):
		cout << "\033[38;2;142;69;133mP\033[0m";
		break;
	case static_cast<int>(TokenID::MR_GREEN):
		cout << "\033[38;2;0;255;0mG\033[0m";
		break;
	case static_cast<int>(TokenID::MRS_PEACOCK):
		cout << "\033[38;2;51;161;201mP\033[0m";
		break;
	case static_cast<int>(TokenID::MRS_WHITE):
		cout << "\033[38;2;255;127;80mW\033[0m";
		break;
	}
}