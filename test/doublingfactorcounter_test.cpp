#include "CppUTest/TestHarness.h"
#include "doubling-factor-counter.h"
#include "wining-state.h"
#include "player-hand.h"
#include "tile-functor.h"
#include "pattern-define.h"
#include "tile-define.h"
#include "wining-hand.h"
#include "pair.h"
#include "meld.h"
#include "doubling-factor-report.h"
#include "doubling-factor-table.h"

using namespace mahjong;

TEST_GROUP(DoublingFactorCounterTest)
{
	PlayerHand h;
	WiningState s;
	DoubligFactorCounter c { h, s };

	void addPair(Tile tile)
	{
		h.add(tile);
		h.add(tile);
	}

	void addTriplet(Tile tile, bool is_open)
	{
		h.add(tile);
		h.add(tile);
		h.add(tile);

		if (is_open)
		{
			h.bindTriplet({ tile, tile, tile });
		}
	}

	void addSequence(Tile tile, bool is_open)
	{
		auto next_tile = NextTile()(tile);
		auto last_tile = NextTile()(next_tile);

		h.add(tile);
		h.add(next_tile);
		h.add(last_tile);

		if (is_open)
		{
			h.bindSequence({ tile, next_tile, last_tile });
		}
	}

	void addQuad(Tile tile, bool is_open)
	{
		h.add(tile);
		h.add(tile);
		h.add(tile);
		h.add(tile);

		if (is_open)
		{
			h.bindOpenQuad({ tile,tile,tile,tile });
		}
		else
		{
			h.bindClosedQuad({ tile,tile,tile,tile });
		}
	}
};

TEST(DoublingFactorCounterTest, SevenPairs)
{
	addPair(Tile::EastWind);
	addPair(Tile::NorthWind);
	addPair(Tile::ThreeOfCharacters);
	addPair(Tile::NineOfCircles);
	addPair(Tile::OneOfBamboos);
	addPair(Tile::GreenDragon);
	addPair(Tile::OneOfCharacters);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::SevenPairs));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Not_SevenPairs_FourSuit)
{
	addPair(Tile::EastWind);
	addPair(Tile::NorthWind);
	addPair(Tile::ThreeOfCharacters);
	addPair(Tile::NineOfCircles);
	addPair(Tile::OneOfBamboos);
	addPair(Tile::GreenDragon);
	addPair(Tile::GreenDragon);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::SevenPairs));
}

TEST(DoublingFactorCounterTest, NoPointsHand)
{
	addPair(Tile::SouthWind);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addSequence(Tile::SevenOfCharacters, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::NoPointsHand));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Not_NoPointsHand_Open)
{
	addPair(Tile::SouthWind);
	addSequence(Tile::OneOfCharacters, true);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addSequence(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::NoPointsHand));
}

TEST(DoublingFactorCounterTest, Not_NoPointsHand_NotMultiWait)
{
	addPair(Tile::SouthWind);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);

	h.add(Tile::NineOfCircles);
	h.add(Tile::EightOfCircles);
	h.add(Tile::SevenOfCircles);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::NoPointsHand));
}

TEST(DoublingFactorCounterTest, Not_NoPointsHand_DragonPair)
{
	addPair(Tile::WhiteDragon);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addSequence(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::NoPointsHand));
}

TEST(DoublingFactorCounterTest, NoPointsHand_ScoringWindPair)
{
	s.setRoundWind(Tile::EastWind);

	addPair(Tile::EastWind);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addSequence(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::NoPointsHand));
}

TEST(DoublingFactorCounterTest, OneSetOfIdenticalSequences)
{
	addPair(Tile::SouthWind);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);
	addTriplet(Tile::NineOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::OneSetOfIdenticalSequences));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Not_OneSetOfIdenticalSequences_Open)
{
	addPair(Tile::SouthWind);	
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);	
	addTriplet(Tile::NineOfBamboos, true);

	s.selfDrawn();
	auto r = c.report();

	CHECK(r.patterns.empty());
}

TEST(DoublingFactorCounterTest, Not_OneSetOfIdenticalSequences_ThreeClosedTriplets)
{
	addPair(Tile::SouthWind);	
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);	
	addTriplet(Tile::NineOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeClosedTriplets));
}

TEST(DoublingFactorCounterTest, ThreeColourStraights)
{
	addPair(Tile::WestWind);
	addSequence(Tile::TwoOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addSequence(Tile::SevenOfCharacters, true);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeColourStraights));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Straight)
{
	addPair(Tile::WestWind);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::FourOfCharacters, false);
	addSequence(Tile::SevenOfCharacters, false);
	addTriplet(Tile::OneOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::Straight));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Straight_Open)
{
	addPair(Tile::WestWind);
	addSequence(Tile::OneOfCharacters, true);
	addSequence(Tile::FourOfCharacters, false);
	addSequence(Tile::SevenOfCharacters, false);
	addTriplet(Tile::OneOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::Straight));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, TwoSetsOfIdenticalSequences)
{
	addPair(Tile::WhiteDragon);
	addSequence(Tile::TwoOfCharacters, false);
	addSequence(Tile::TwoOfCharacters, false);
	addSequence(Tile::ThreeOfCircles, false);
	addSequence(Tile::ThreeOfCircles, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::TwoSetsOfIdenticalSequences));
	CHECK_EQUAL(0, r.patterns.count(Pattern::SevenPairs));
	CHECK_EQUAL(3, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, TwoSetsOfIdenticalSequences_SameFourSequences)
{
	addPair(Tile::SouthWind);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfCircles, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::TwoSetsOfIdenticalSequences));
}

TEST(DoublingFactorCounterTest, Not_TwoSetsOfIdenticalSequences_Open)
{
	addPair(Tile::WestWind);
	addSequence(Tile::TwoOfCharacters, false);
	addSequence(Tile::TwoOfCharacters, false);
	addSequence(Tile::ThreeOfCircles, false);
	addSequence(Tile::ThreeOfCircles, true);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::TwoSetsOfIdenticalSequences));
}

TEST(DoublingFactorCounterTest, AllTriplets)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::OneOfCharacters, true);
	addTriplet(Tile::TwoOfCircles, true);
	addTriplet(Tile::ThreeOfBamboos, false);
	addTriplet(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTriplets));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, ThreeClosedTriplets)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::OneOfCharacters, false);
	addTriplet(Tile::TwoOfCircles, false);
	addQuad(Tile::ThreeOfBamboos, false);
	addSequence(Tile::SevenOfCharacters, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeClosedTriplets));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Not_ThreeClosedTriplets_WinByDiscard)
{
	addPair(Tile::WestWind);
	addSequence(Tile::SevenOfCharacters, false);
	addTriplet(Tile::OneOfCharacters, false);
	addTriplet(Tile::TwoOfCircles, false);
	addTriplet(Tile::ThreeOfBamboos, false);
		
	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::ThreeClosedTriplets));
}

TEST(DoublingFactorCounterTest, ThreeClosedTriplets_WinByDiscard_OtherMeld)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::OneOfCharacters, false);
	addTriplet(Tile::TwoOfCircles, false);
	addSequence(Tile::ThreeOfBamboos, false);
	addTriplet(Tile::ThreeOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeClosedTriplets));
}

TEST(DoublingFactorCounterTest, AllTriplets_ThreeClosedTriplets)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::OneOfCharacters, false);
	addTriplet(Tile::TwoOfCircles, false);
	addTriplet(Tile::ThreeOfBamboos, false);
	addTriplet(Tile::SevenOfCharacters, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTriplets));
	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeClosedTriplets));
	CHECK_EQUAL(4, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, ThreeColourTriplets)
{
	addPair(Tile::WestWind);
	addSequence(Tile::SevenOfCharacters, true);
	addTriplet(Tile::ThreeOfCharacters, true);
	addTriplet(Tile::ThreeOfCircles, false);
	addQuad(Tile::ThreeOfBamboos, false);	

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeColourTriplets));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, ThreeQuads)
{
	addPair(Tile::WestWind);
	addQuad(Tile::ThreeOfCharacters, false);
	addQuad(Tile::FourOfCircles, true);
	addQuad(Tile::FiveOfBamboos, true);
	addSequence(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeQuads));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, ThreeQuads_ThreeClosedTriplets_AllTriplets)
{
	addPair(Tile::WestWind);
	addQuad(Tile::ThreeOfCharacters, false);
	addQuad(Tile::FourOfCircles, false);
	addQuad(Tile::FiveOfBamboos, false);
	addTriplet(Tile::SevenOfCharacters, true);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeQuads));
	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeClosedTriplets));
	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTriplets));
	CHECK_EQUAL(6, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, AllSimples)
{
	addPair(Tile::ThreeOfCharacters);
	addSequence(Tile::FiveOfCharacters, false);
	addTriplet(Tile::ThreeOfCircles, false);
	addSequence(Tile::ThreeOfBamboos, false);
	addSequence(Tile::SixOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllSimples));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, HonorTiles_Dragon)
{
	addPair(Tile::SouthWind);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addTriplet(Tile::WhiteDragon, true);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::WhiteDragon));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, HonorTiles_Wind)
{
	s.setRoundWind(Tile::EastWind);
	s.setSeatWind(Tile::SouthWind);

	addPair(Tile::WhiteDragon);
	addSequence(Tile::OneOfCharacters, true);
	addSequence(Tile::TwoOfCircles, false);
	addSequence(Tile::TwoOfBamboos, false);
	addTriplet(Tile::EastWind, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::EastWind));
	CHECK_EQUAL(1, r.doubling_factor);

	s.setSeatWind(Tile::EastWind);	
	r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::DoubleEastWind));
	CHECK_EQUAL(2, r.doubling_factor);

	s.setRoundWind(Tile::SouthWind);
	r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::EastWind));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, TerminalOrHonorInEachSet)
{
	addPair(Tile::WhiteDragon);
	addTriplet(Tile::WestWind, false);
	addTriplet(Tile::OneOfCharacters, false);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::SevenOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::TerminalOrHonorInEachSet));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, TerminalOrHonorInEachSet_Open)
{
	addPair(Tile::WhiteDragon);
	addTriplet(Tile::WestWind, true);
	addTriplet(Tile::OneOfCharacters, false);
	addSequence(Tile::OneOfCharacters, false);
	addSequence(Tile::SevenOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::TerminalOrHonorInEachSet));
	CHECK_EQUAL(1, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, AllTerminalsAndHornors_AllTriplets)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::NorthWind, true);
	addTriplet(Tile::OneOfCharacters, true);
	addTriplet(Tile::NineOfCircles, false);
	addTriplet(Tile::NineOfBamboos, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTerminalsAndHornors));
	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTriplets));
	CHECK_EQUAL(0, r.patterns.count(Pattern::TerminalOrHonorInEachSet));
	CHECK_EQUAL(4, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, AllTerminalsAndHornors_SevenPairs)
{
	addPair(Tile::OneOfCharacters);
	addPair(Tile::NineOfCharacters);
	addPair(Tile::OneOfCircles);
	addPair(Tile::NineOfCircles);
	addPair(Tile::NineOfBamboos);
	addPair(Tile::WhiteDragon);
	addPair(Tile::WestWind);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTerminalsAndHornors));
	CHECK_EQUAL(1, r.patterns.count(Pattern::SevenPairs));
	CHECK_EQUAL(0, r.patterns.count(Pattern::TerminalOrHonorInEachSet));
	CHECK_EQUAL(4, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, TerminalInEachSet)
{
	addPair(Tile::NineOfCharacters);
	addSequence(Tile::OneOfCharacters, false);
	addTriplet(Tile::NineOfCircles, false);
	addSequence(Tile::OneOfBamboos, false);
	addSequence(Tile::SevenOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::TerminalInEachSet));
	CHECK_EQUAL(0, r.patterns.count(Pattern::TerminalOrHonorInEachSet));
	CHECK_EQUAL(3, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, TerminalInEachSet_Open)
{
	addPair(Tile::NineOfCharacters);
	addSequence(Tile::OneOfCharacters, true);
	addTriplet(Tile::NineOfCircles, false);
	addSequence(Tile::OneOfBamboos, false);
	addSequence(Tile::SevenOfBamboos, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::TerminalInEachSet));
	CHECK_EQUAL(0, r.patterns.count(Pattern::TerminalOrHonorInEachSet));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Not_TerminalInEachSet_NotTerminalPair)
{
	addPair(Tile::WestWind);
	addSequence(Tile::OneOfCharacters, false);
	addTriplet(Tile::NineOfCircles, false);
	addSequence(Tile::OneOfBamboos, false);
	addSequence(Tile::SevenOfBamboos, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::TerminalInEachSet));
}

TEST(DoublingFactorCounterTest, LittleThreeDragons)
{
	addPair(Tile::WhiteDragon);
	addTriplet(Tile::RedDragon, true);
	addTriplet(Tile::GreenDragon, false);
	addSequence(Tile::TwoOfCharacters, false);
	addSequence(Tile::SixOfBamboos, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::LittleThreeDragons));
	CHECK_EQUAL(1, r.patterns.count(Pattern::RedDragon));
	CHECK_EQUAL(1, r.patterns.count(Pattern::GreenDragon));
	CHECK_EQUAL(4, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, HalfFlush)
{
	addPair(Tile::WhiteDragon);
	addTriplet(Tile::WestWind, false);
	addSequence(Tile::OneOfCharacters, false);
	addTriplet(Tile::FiveOfCharacters, false);
	addSequence(Tile::SixOfCharacters, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::HalfFlush));
	CHECK_EQUAL(3, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, HalfFlush_Open)
{
	addPair(Tile::WhiteDragon);
	addTriplet(Tile::WestWind, true);
	addSequence(Tile::OneOfCharacters, false);
	addTriplet(Tile::FiveOfCharacters, false);
	addSequence(Tile::SixOfCharacters, false);

	s.winByDiscard();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::HalfFlush));
	CHECK_EQUAL(2, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Flush)
{
	addPair(Tile::OneOfCharacters);
	addTriplet(Tile::TwoOfCharacters, true);
	addTriplet(Tile::FourOfCharacters, false);
	addSequence(Tile::SixOfCharacters, false);
	addSequence(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::Flush));
	CHECK_EQUAL(6, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, FourClosedTriplets)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::OneOfCharacters, false);
	addTriplet(Tile::TwoOfCircles, false);
	addQuad(Tile::ThreeOfBamboos, false);
	addTriplet(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::FourClosedTriplets));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, ThirteenOrphans)
{
	h.add(Tile::OneOfCharacters);
	h.add(Tile::NineOfCharacters);
	h.add(Tile::OneOfCircles);
	h.add(Tile::NineOfCircles);
	h.add(Tile::OneOfBamboos);
	h.add(Tile::NineOfBamboos);
	h.add(Tile::EastWind);
	h.add(Tile::SouthWind);
	h.add(Tile::WestWind);
	h.add(Tile::NorthWind);
	h.add(Tile::WhiteDragon);
	h.add(Tile::GreenDragon);
	addPair(Tile::RedDragon);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::ThirteenOrphans));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, NineGates)
{
	addTriplet(Tile::OneOfCharacters, false);
	h.add(Tile::TwoOfCharacters);
	h.add(Tile::ThreeOfCharacters);
	h.add(Tile::FourOfCharacters);
	h.add(Tile::FiveOfCharacters);
	h.add(Tile::SixOfCharacters);
	h.add(Tile::SevenOfCharacters);
	h.add(Tile::EightOfCharacters);
	addTriplet(Tile::NineOfCharacters, false);

	h.add(Tile::NineOfCharacters);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::NineGates));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, Not_NineGates_Open)
{
	addTriplet(Tile::OneOfCharacters, false);
	h.add(Tile::TwoOfCharacters);
	h.add(Tile::ThreeOfCharacters);
	h.add(Tile::FourOfCharacters);
	h.add(Tile::FiveOfCharacters);
	h.add(Tile::SixOfCharacters);
	h.add(Tile::SevenOfCharacters);
	h.add(Tile::EightOfCharacters);
	addTriplet(Tile::NineOfCharacters, true);

	h.add(Tile::NineOfCharacters);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(0, r.patterns.count(Pattern::NineGates));
}

TEST(DoublingFactorCounterTest, AllGreen)
{
	addPair(Tile::GreenDragon);
	addTriplet(Tile::TwoOfBamboos, false);
	addSequence(Tile::TwoOfBamboos, false);
	addTriplet(Tile::SixOfBamboos, false);
	addTriplet(Tile::EightOfBamboos, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllGreen));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, AllHonors)
{
	addPair(Tile::EastWind);
	addTriplet(Tile::SouthWind, false);
	addTriplet(Tile::WestWind, false);
	addTriplet(Tile::WhiteDragon, false);
	addTriplet(Tile::GreenDragon, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllHonors));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, AllHonors_SevenPairsForm)
{
	addPair(Tile::EastWind);
	addPair(Tile::SouthWind);
	addPair(Tile::WestWind);
	addPair(Tile::NorthWind);
	addPair(Tile::WhiteDragon);
	addPair(Tile::RedDragon);
	addPair(Tile::GreenDragon);	

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllHonors));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, AllTerminals)
{
	addPair(Tile::OneOfCharacters);
	addTriplet(Tile::NineOfCharacters, false);
	addTriplet(Tile::OneOfCircles, false);
	addTriplet(Tile::OneOfBamboos, false);
	addTriplet(Tile::NineOfBamboos, false);	

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTerminals));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, BigThreeDragons)
{
	addPair(Tile::WestWind);
	addTriplet(Tile::WhiteDragon, true);
	addTriplet(Tile::RedDragon, false);
	addTriplet(Tile::GreenDragon, false);
	addSequence(Tile::TwoOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::BigThreeDragons));
	CHECK_EQUAL(0, r.patterns.count(Pattern::WhiteDragon));
	CHECK_EQUAL(0, r.patterns.count(Pattern::RedDragon));
	CHECK_EQUAL(0, r.patterns.count(Pattern::GreenDragon));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, LittleFourWinds)
{
	addPair(Tile::EastWind);
	addTriplet(Tile::SouthWind, false);
	addTriplet(Tile::WestWind, false);
	addTriplet(Tile::NorthWind, false);
	addSequence(Tile::TwoOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::LittleFourWinds));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, BigFourWinds)
{
	addPair(Tile::OneOfBamboos);
	addTriplet(Tile::EastWind, false);
	addTriplet(Tile::SouthWind, false);
	addTriplet(Tile::WestWind, false);
	addTriplet(Tile::NorthWind, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::BigFourWinds));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, FourQuads)
{
	addPair(Tile::WestWind);
	addQuad(Tile::ThreeOfCharacters, false);
	addQuad(Tile::FourOfCircles, true);
	addQuad(Tile::FiveOfBamboos, true);
	addQuad(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::FourQuads));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, FourQuads_FourClosedTriplets)
{
	addPair(Tile::WestWind);
	addQuad(Tile::ThreeOfCharacters, false);
	addQuad(Tile::FourOfCircles, false);
	addQuad(Tile::FiveOfBamboos, false);
	addQuad(Tile::SevenOfCharacters, false);

	s.selfDrawn();
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::FourQuads));
	CHECK_EQUAL(1, r.patterns.count(Pattern::FourClosedTriplets));
	CHECK_EQUAL(100, r.doubling_factor);
}

TEST(DoublingFactorCounterTest, MaxDoublingFactor)
{
	s.setRoundWind(Tile::EastWind);
	s.setSeatWind(Tile::EastWind);

	addPair(Tile::WhiteDragon);
	addQuad(Tile::GreenDragon, false);
	addQuad(Tile::RedDragon, false);
	addQuad(Tile::OneOfBamboos, false);
	addTriplet(Tile::EastWind, false);

	s.addBonusTile(Tile::GreenDragon);
	s.addBonusTile(Tile::GreenDragon);
	s.addBonusTile(Tile::GreenDragon);
	s.addBonusTile(Tile::RedDragon);
	s.addBonusTile(Tile::RedDragon);

	s.addBonusTile(Tile::RedDragon);
	s.addBonusTile(Tile::OneOfBamboos);
	s.addBonusTile(Tile::OneOfBamboos);
	s.addBonusTile(Tile::OneOfBamboos);
	s.addBonusTile(Tile::OneOfBamboos);

	h.doubleReady();

	WinByDiscardSituation situation;
	situation.is_last_discard = true;

	s.winByDiscard(situation);
	auto r = c.report();

	CHECK_EQUAL(1, r.patterns.count(Pattern::DoubleReady));
	CHECK_EQUAL(1, r.patterns.count(Pattern::LastDiscard));
	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTriplets));	
	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeClosedTriplets));
	CHECK_EQUAL(1, r.patterns.count(Pattern::ThreeQuads));
	CHECK_EQUAL(1, r.patterns.count(Pattern::DoubleEastWind));
	CHECK_EQUAL(1, r.patterns.count(Pattern::GreenDragon));
	CHECK_EQUAL(1, r.patterns.count(Pattern::RedDragon));
	CHECK_EQUAL(1, r.patterns.count(Pattern::LittleThreeDragons));
	CHECK_EQUAL(1, r.patterns.count(Pattern::AllTerminalsAndHornors));
	CHECK_EQUAL(1, r.patterns.count(Pattern::HalfFlush));
	CHECK_EQUAL(40, r.bonus_tile_count);

	CHECK_EQUAL(60, r.doubling_factor);	
}
