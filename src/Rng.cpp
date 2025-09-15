#include "Rng.h"
extern std::mt19937 rng = std::mt19937(std::random_device{}());

int RNG::GetVal()
{
	return Range(rng);
}
void RNG::ChangeRange(int min, int max)
{
	Range = std::uniform_int_distribution<int>{ min, max };
}

extern RNG Chance = { 1 , 100 };