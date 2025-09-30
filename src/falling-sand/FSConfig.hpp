#pragma once
#include <stdint.h>
#include <string>
#include <string_view>
#include "chili/Colors.hpp"



namespace sand {
	struct ParticleData {
		int8_t id;
		std::string name;
		// I do not want to allocate the color range on the heap
		chili::Color colorRange[5];
		int8_t colorCnt;
		int8_t burnChance;
	};

	class FSConfig {
	public:
		static constexpr int maxParticleTypes = 256;
		const ParticleData& GetParticle(const std::string_view name) const;
		const ParticleData& GetParticle(const int8_t id) const;
		int TypeCount() const;
	private:
		ParticleData types[maxParticleTypes];
	};
}
