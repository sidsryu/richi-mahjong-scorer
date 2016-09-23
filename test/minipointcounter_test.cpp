#include "CppUTest/TestHarness.h"
#include "minipoint-counter.h"
#include "wining-state.h"
#include "tile-define.h"
#include "tile-functor.h"
#include "wining-hand.h"
#include "pair.h"
#include "meld.h"

using namespace std;

TEST_GROUP(MinipointCounterTest)
{
	WiningState s;
	WiningHand h;
	MinipointCounter m { h, s };

	void addPair(Tile tile)
	{
		h.pairs.push_back(vector<Tile>{ tile, tile });
		h.last_tile = tile;
	}

	void addPon(Tile tile, bool is_open)
	{
		h.melds.push_back({ { tile, tile, tile }, is_open });
		h.last_tile = tile;
	}

	void addKan(Tile tile, bool is_open)
	{
		h.melds.push_back({ { tile, tile, tile, tile }, is_open });
		h.last_tile = tile;
	}

	void addChii(Tile tile, bool is_open)
	{
		auto next_tile = NextTile()(tile);
		auto last_tile = NextTile()(next_tile);

		vector<Tile> tiles {
			tile,
			next_tile,
			last_tile,
		};
		h.melds.push_back({ tiles, is_open });
		h.last_tile = last_tile;
	}

	void addNoPointHand()
	{
		addPair(Tile::SouthWind);
		addChii(Tile::OneOfCharacters, false);
		addChii(Tile::TwoOfCircles, false);
		addChii(Tile::TwoOfBamboos, false);
		addChii(Tile::SevenOfCharacters, false);
	}

	void addOpenNoPointHand()
	{
		// Terminal or honor in each set
		addPair(Tile::SouthWind);
		addChii(Tile::OneOfCharacters, false);
		addChii(Tile::OneOfCircles, true);
		addChii(Tile::OneOfBamboos, true);
		addChii(Tile::SevenOfCharacters, false);
	}

	void addOpenNoPointsHandWithoutPair()
	{
		addChii(Tile::TwoOfCharacters, false);
		addChii(Tile::TwoOfCircles, true);
		addChii(Tile::TwoOfBamboos, true);
		addChii(Tile::SevenOfCharacters, false);
	}

	void addOpenNoPointsHandWithoutOneMeld()
	{
		addPair(Tile::SouthWind);		
		addChii(Tile::TwoOfCircles, true);
		addChii(Tile::TwoOfBamboos, true);		
		addChii(Tile::TwoOfCharacters, false);
	}
};

TEST(MinipointCounterTest, BasicPoints_OpenNoPointsHand)
{
	addOpenNoPointHand();
	s.setRon();

	CHECK_EQUAL(30, m.total(false));
	CHECK_EQUAL(30, m.total());
}

TEST(MinipointCounterTest, ClosedRon)
{
	addNoPointHand();
	s.setRon();

	CHECK_EQUAL(30, m.total(false));
	CHECK_EQUAL(30, m.total());
}

TEST(MinipointCounterTest, SelfPick)
{
	addOpenNoPointHand();
	CHECK_EQUAL(22, m.total(false));
	CHECK_EQUAL(30, m.total());
}

TEST(MinipointCounterTest, NoPointsHandSelfPick)
{
	addNoPointHand();
	CHECK_EQUAL(20, m.total(false));
	CHECK_EQUAL(20, m.total());
}

TEST(MinipointCounterTest, SevenPairs)
{
	addPair(Tile::EastWind);
	addPair(Tile::NorthWind);
	addPair(Tile::ThreeOfCharacters);
	addPair(Tile::NineOfCircles);
	addPair(Tile::OneOfBamboos);
	addPair(Tile::GreenDragon);
	addPair(Tile::OneOfCharacters);

	CHECK_EQUAL(25, m.total(false));
	CHECK_EQUAL(25, m.total());
}

TEST(MinipointCounterTest, EdgeWait)
{
	addOpenNoPointHand();
	s.setRon();

	h.last_tile = Tile::ThreeOfCharacters;
	CHECK_EQUAL(22, m.total(false));

	h.last_tile = Tile::SevenOfCharacters;
	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, ClosedWait)
{
	addOpenNoPointHand();
	s.setRon();

	h.last_tile = Tile::EightOfCharacters;
	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, PairWait)
{
	addOpenNoPointHand();
	s.setRon();

	h.last_tile = Tile::SouthWind;
	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, ChooseHighestPointsWait)
{
	addPair(Tile::SouthWind);
	addChii(Tile::OneOfCharacters, false);
	addChii(Tile::OneOfCircles, true);
	addChii(Tile::OneOfBamboos, true);
	addChii(Tile::ThreeOfCharacters, false);
	s.setRon();

	h.last_tile = Tile::ThreeOfCharacters;
	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, ChooseHighestPointsWait_AnotherHandOrder)
{
	addPair(Tile::SouthWind);
	addChii(Tile::ThreeOfCharacters, false);
	addChii(Tile::OneOfCharacters, false);
	addChii(Tile::OneOfCircles, true);
	addChii(Tile::OneOfBamboos, true);	
	s.setRon();

	h.last_tile = Tile::ThreeOfCharacters;
	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, DragonPair)
{
	addPair(Tile::WhiteDragon);
	addOpenNoPointsHandWithoutPair();
	s.setRon();
	
	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, OwnWindPair)
{
	s.setRountWind(Tile::EastWind);
	s.setOwnWind(Tile::SouthWind);

	addPair(Tile::SouthWind);
	addOpenNoPointsHandWithoutPair();
	s.setRon();

	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, RoundWindPair)
{
	s.setRountWind(Tile::EastWind);
	s.setOwnWind(Tile::SouthWind);
	
	addPair(Tile::EastWind);
	addOpenNoPointsHandWithoutPair();
	s.setRon();

	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, OwnAndRoundWindPair)
{
	s.setRountWind(Tile::EastWind);
	s.setOwnWind(Tile::EastWind);

	addPair(Tile::EastWind);
	addOpenNoPointsHandWithoutPair();
	s.setRon();

	CHECK_EQUAL(24, m.total(false));
}

TEST(MinipointCounterTest, SimpleOpenPon)
{
	addPon(Tile::EightOfCharacters, true);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();

	CHECK_EQUAL(22, m.total(false));
}

TEST(MinipointCounterTest, SimpleClosePon)
{
	addPon(Tile::EightOfCircles, false);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();
	
	CHECK_EQUAL(24, m.total(false));
}

TEST(MinipointCounterTest, SimpleOpenKan)
{
	addKan(Tile::EightOfCircles, true);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();
	
	CHECK_EQUAL(28, m.total(false));
}

TEST(MinipointCounterTest, SimpleCloseKan)
{
	addKan(Tile::EightOfCircles, false);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();
	
	CHECK_EQUAL(36, m.total(false));
}

TEST(MinipointCounterTest, HornorOrTerminalOpenPon)
{
	addPon(Tile::WhiteDragon, true);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();
	
	CHECK_EQUAL(24, m.total(false));
}

TEST(MinipointCounterTest, HornorOrTerminalClosePon)
{
	addPon(Tile::WhiteDragon, false);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();

	CHECK_EQUAL(28, m.total(false));
}

TEST(MinipointCounterTest, HornorOrTerminalOpenKan)
{
	addKan(Tile::WhiteDragon, true);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();

	CHECK_EQUAL(36, m.total(false));
}

TEST(MinipointCounterTest, HornorOrTerminalCloseKan)
{
	addKan(Tile::WhiteDragon, false);
	addOpenNoPointsHandWithoutOneMeld();
	s.setRon();

	CHECK_EQUAL(52, m.total(false));
}

TEST(MinipointCounterTest, HighestPoints)
{
	s.setRountWind(Tile::EastWind);
	s.setOwnWind(Tile::EastWind);

	addKan(Tile::OneOfCharacters, false);
	addKan(Tile::RedDragon, false);
	addKan(Tile::SouthWind, true);
	addChii(Tile::SevenOfCircles, false);
	addPair(Tile::EastWind);

	CHECK_EQUAL(108, m.total(false));
	CHECK_EQUAL(110, m.total());
}
