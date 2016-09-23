#pragma once

#include <set>
#include <functional>
#include <vector>

enum class Tile;
class Meld;
class TileHolder;

class PlayerHand
{
	using BindTiles = std::vector<Tile>;

public:
	void add(Tile tile);
	void remove(Tile tile);
	void bindPon(BindTiles tiles);
	void bindKan(BindTiles tiles);
	void bindCloseKan(BindTiles tiles);
	void bindChii(BindTiles tiles);

	bool isClaim() const;
	Tile lastTile() const;
	TileHolder makeHandHolder() const;

private:
	Tile last_tile;
	std::multiset<Tile> tiles;
	std::vector<Meld> melds;
};
