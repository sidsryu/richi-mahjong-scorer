#pragma once

#include <vector>
#include <memory>
#include <set>
#include <functional>

enum class Pattern;
struct WiningHand;
class WiningState;
class HandRecognizer;
class Pair;
class Meld;

class HandComputer
{
	using WiningHands = std::vector<WiningHand>;
	using HandRecognizerPtr = std::unique_ptr<HandRecognizer>;

public:
	HandComputer(const WiningState& state, const WiningHands& hands);
	virtual ~HandComputer();

	void compute();
	void each(std::function<void(Pattern)> fn) const;
	
private:
	void resetRecognizer();
	void check(const WiningHand& hand);
	void check(const Pair& pair);
	void check(const Meld& meld);
	void recognize();

private:
	const WiningState& state;
	const WiningHands& hands;

	std::vector<HandRecognizerPtr> recognizers;
	std::set<Pattern> highest_patterns;
};
