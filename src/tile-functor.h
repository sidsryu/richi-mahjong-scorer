#pragma once

enum class Tile;

struct IsSame
{
	bool operator()(const Tile& lhv, const Tile& rhv) const;
};

struct IsTerminal
{
	bool operator()(const Tile& tile) const;
};

struct IsDragon
{
	bool operator()(const Tile& tile) const;
};

struct NextTile
{
	Tile operator()(const Tile& tile) const;
};
