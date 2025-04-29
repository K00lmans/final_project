#pragma once
#include <array>
#include <string>
#include <unordered_set>

namespace cards {

extern const std::array<std::string, 6> suspects;
extern const std::array<std::string, 7> weapons;
extern const std::array<std::string, 9> rooms;
extern const std::array<std::string, 22> all_cards;
extern const std::unordered_set<std::string> cards_set;
extern const std::unordered_set<std::string> suspects_set;

}

