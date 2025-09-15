#pragma once
#include <random>
class RNG
{
public:
	RNG(int min , int max)
	{
		Range = std::uniform_int_distribution<int> { min, max};
	}
	~RNG()
	{
	}
	int GetVal();
	void ChangeRange(int min, int max);
private:
	std::uniform_int_distribution<int> Range;
};