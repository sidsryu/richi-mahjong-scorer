#pragma once

#include <vector>

enum class Tile;
class Pair;
class Meld;

struct WiningHand
{
	std::vector<Pair> pairs;
	std::vector<Meld> melds;
	std::vector<Tile> tiles;
};
