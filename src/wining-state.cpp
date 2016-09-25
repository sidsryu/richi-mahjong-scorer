#include "wining-state.h"
#include "tile-define.h"
#include <cassert>

WiningState::WiningState()
	: round_wind(Tile::EastWind)
	, seat_wind(Tile::EastWind)	
{}

void WiningState::claim()
{
	is_claim = true;
}

void WiningState::readyHand()
{
	assert(!is_double_ready);
	is_ready_hand = true;
}

void WiningState::doubleReady()
{
	assert(!is_ready_hand);
	is_double_ready = true;
}

void WiningState::winByDiscard(Tile tile, WinByDiscardSituation situation) 
{
	is_win_by_discard = true;
	last_tile = tile;

	is_one_shot = situation.is_one_shot;
	is_last_discard = situation.is_last_discard;
	is_robbing_quad = situation.is_robbing_quad;
	assert(!is_last_discard || !is_robbing_quad);
}

void WiningState::selfDrawn(Tile tile, SelfDrawnSituation situation)
{
	is_win_by_discard = false;
	last_tile = tile;

	is_one_shot = situation.is_one_shot;
	is_last_tile_from_the_wall = situation.is_last_wall;
	is_dead_wall_draw = situation.is_dead_wall;
	assert(!is_last_tile_from_the_wall || !is_dead_wall_draw);
}

void WiningState::setRountWind(Tile tile)
{
	round_wind = tile;
}

void WiningState::setSeatWind(Tile tile)
{
	seat_wind = tile;
}

bool WiningState::isClosedHand() const
{
	return !is_claim;
}

bool WiningState::isReadyHand() const
{
	return is_ready_hand;
}

bool WiningState::isDoubleReady() const
{
	return is_double_ready;
}

bool WiningState::isWinByDiscard() const
{
	return is_win_by_discard;
}

bool WiningState::isSelfDrawn() const
{
	return !is_win_by_discard;
}

Tile WiningState::roundWind() const
{
	return round_wind;
}

Tile WiningState::seatWind() const
{
	return seat_wind;
}

Tile WiningState::lastTile() const
{
	return last_tile;
}

bool WiningState::isOneShot() const
{
	return is_one_shot;
}

bool WiningState::isLastDiscard() const
{
	return is_last_discard;
}

bool WiningState::isLastTileFromTheWall() const
{
	return is_last_tile_from_the_wall;
}

bool WiningState::isDeadWallDraw() const
{
	return is_dead_wall_draw;
}

bool WiningState::isRobbinQuad() const
{
	return is_robbing_quad;
}
