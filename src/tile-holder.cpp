#include "tile-holder.h"
#include "type-define.h"
#include "tile-functor.h"
#include <algorithm>
#include <cassert>

using namespace std;

void TileHolder::add(Tile tile)
{
	tiles.push_back(tile);
	sort(tiles.begin(), tiles.end());	
}

void TileHolder::add(Meld meld)
{
	melds.push_back(meld);
	sort(melds.begin(), melds.end(), [](const auto& lhs, const auto& rhs) {
		return lhs.tiles.front() < rhs.tiles.front();
	});
}

bool TileHolder::empty() const
{
	return tiles.empty() && melds.empty();
}

bool TileHolder::isNextTilePair()  const
{
	if (tiles.size() < 2) return false;
	return IsSame()(tiles.front(), tiles.at(1));
}

bool TileHolder::isNextTilePonOrKan() const
{
	if (tiles.empty())
	{
		const auto& meld = melds.front();
		return IsSame()(meld.tiles[0], meld.tiles[1]);
	}

	if (tiles.size() < 3) return false;
	return IsSame()(tiles.front(), tiles.at(1)) && IsSame()(tiles.front(), tiles.at(2));
}

bool TileHolder::isNextTileChii()  const
{
	if (tiles.empty())
	{
		const auto& meld = melds.front();
		return !IsSame()(meld.tiles[0], meld.tiles[1]);
	}

	if (tiles.size() < 3) return false;

	auto tile = tiles.front();
	auto count = 1;

	for (auto it : tiles)
	{
		if (IsSame()(NextTile()(tile), it))
		{
			tile = it;
			count++;
		}

		if (count == 3)
		{
			return true;
		}
	}

	return false;
}

Pair TileHolder::popNextPair()
{
	assert(2 <= tiles.size());

	vector<Tile> pair;

	pair.push_back(tiles.front());
	tiles.erase(tiles.begin());

	pair.push_back(tiles.front());
	tiles.erase(tiles.begin());

	return pair;
}

Meld TileHolder::popNextPonOrKan()
{
	if (tiles.empty())
	{
		for (auto it = melds.begin(); it != melds.end(); it++)
		{
			if (IsSame()(it->tiles[0], it->tiles[1]))
			{
				auto meld = *it;
				melds.erase(it);

				return meld;
			}
		}
	}

	if (3 <= tiles.size())
	{
		if (IsSame()(tiles.front(), tiles.at(1)) && IsSame()(tiles.front(), tiles.at(2)))
		{
			vector<Tile> pon;

			pon.push_back(tiles.front());
			tiles.erase(tiles.begin());

			pon.push_back(tiles.front());
			tiles.erase(tiles.begin());

			pon.push_back(tiles.front());
			tiles.erase(tiles.begin());

			return { pon, false };
		}
	}

	assert(false);
	return { {}, false };
}

Meld TileHolder::popNextChii()
{
	if (tiles.empty())
	{
		for (auto it = melds.begin(); it != melds.end(); it++)
		{
			if (!IsSame()(it->tiles[0], it->tiles[1]))
			{
				auto meld = *it;
				melds.erase(it);

				return meld;
			}
		}
	}

	if (3 <= tiles.size())
	{
		vector<Tile> chii;

		auto tile = tiles.front();
		auto count = 0;

		for (auto it = tiles.begin(); it != tiles.end();)
		{
			if (count == 0 || IsSame()(NextTile()(tile), *it))
			{
				chii.push_back(*it);
				tile = *it;
				it = tiles.erase(it);
				count++;

				if (count == 3) break;
			}
			else
			{
				it++;
			}
		}

		if (3 == chii.size())
		{
			return { chii, false };
		}
	}

	assert(false);
	return { {}, false };
}
