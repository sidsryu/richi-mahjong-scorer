#include "wining-hand-collator.h"
#include "wining-hand.h"
#include "pair.h"
#include "meld.h"
#include "player-hand.h"
#include "tile-holder.h"

using namespace std;

WiningHandCollator::WiningHandCollator(const PlayerHand& hand)
	: hand(hand)
{}

WiningHandCollator::WiningHands WiningHandCollator::collate()
{
	wining_hands.clear();

	auto seed_holder = hand.makeHandHolder();
	auto seed_hand = WiningHand();
	backtrack(seed_hand, seed_holder);

	return wining_hands;
}

void WiningHandCollator::backtrack(const WiningHand& extended_hand, const TileHolder& extended_holder)
{
	if (extended_holder.empty())
	{
		wining_hands.push_back(extended_hand);
		return;
	}

	if (extended_holder.isNextTilePair())
	{
		backtrackPair(extended_hand, extended_holder);
	}

	if (extended_holder.isNextTileTripletOrQuad())
	{
		backtrackTripletOrQuad(extended_hand, extended_holder);
	}

	if (extended_holder.isNextTileSequence())
	{
		backtrackSequence(extended_hand, extended_holder);
	}
}

void WiningHandCollator::backtrackPair(WiningHand extended_hand, TileHolder extended_holder)
{
	auto pair = extended_holder.popNextPair();
	extended_hand.pairs.push_back(pair);

	backtrack(extended_hand, extended_holder);
}

void WiningHandCollator::backtrackTripletOrQuad(WiningHand extended_hand, TileHolder extended_holder)
{
	auto meld = extended_holder.popNextTripletOrQuad();
	extended_hand.melds.push_back(meld);

	backtrack(extended_hand, extended_holder);
}

void WiningHandCollator::backtrackSequence(WiningHand extended_hand, TileHolder extended_holder)
{
	auto meld = extended_holder.popNextSequence();
	extended_hand.melds.push_back(meld);

	backtrack(extended_hand, extended_holder);
}
