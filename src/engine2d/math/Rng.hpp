#pragma once
#include <random>

namespace e2d {
	inline std::mt19937 rng = std::mt19937(std::random_device{}());

	inline std::uniform_int_distribution<int> PercentileRange = std::uniform_int_distribution<int>(1, 100);
	class RNG
	{
	public:
		RNG()
		{
			Range = std::uniform_int_distribution<int>{ 0, 1 };
		}
		RNG(int min, int max)
		{
			Range = std::uniform_int_distribution<int>{ min, max };
		}

		int GetVal()
		{
			return Range(rng);
		}
		void ChangeRange(int min, int max)
		{
			Range = std::uniform_int_distribution<int>{ min, max };
		}
		static int RandomPercentile(){
			return PercentileRange(rng);
		}
	private:
		std::uniform_int_distribution<int> Range;
	};

	// random value from 1 to 100
}