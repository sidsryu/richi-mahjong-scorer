#pragma once

class Pair;
class Meld;

class HandRecognizer
{
public:
	virtual ~HandRecognizer() = default;

	virtual void reset() = 0;
	virtual void check(const Pair& pair) = 0;
	virtual void check(const Meld& meld) = 0;
	virtual void recognize() = 0;
};
