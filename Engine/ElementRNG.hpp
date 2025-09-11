#pragma once
#include "Rng.h"

class ElementRNG {
public:
	constexpr ElementRNG(int elemSize) {

	}
	RNG RdLifeSpan_gas = { 10 , 15 };
	RNG ColorRng = { 0 , 3 };
	RNG XRange = { 0 , hBox.width - 1 };
	mutable RNG SpreadRange = { -2 , 2 };
};