#pragma once
#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include "chili/Colors.hpp"



namespace sand {
	struct ParticleData {
		/* Utility*/
		void AddColor(chili::Color color);
		static constexpr int MaxColorRange = 5;
		/* Variables */
		int8_t id;
		std::string name;
		// I do not want to allocate the color range on the heap
		chili::Color colorRange[MaxColorRange];
		int8_t colorCnt;
		int8_t burnChance;
	};

	/*
	*	This class represents the configuration for the falling-sand engine(It does NOT include graphics settings)
	*	It uses the builder pattern
	*/
	class FSConfig {
	public:
		FSConfig() = default;
		const ParticleData& GetParticle(const std::string_view name) const;
		const ParticleData& GetParticle(const int8_t id) const;
		int TypeCount() const;

		void AddParticleType(const ParticleData& data);
		FSConfig& SetParticleTypes(const std::vector<ParticleData>& typeList);
		FSConfig& SetSpeed(float speed);
		FSConfig& SetUpdateLimit(uint8_t limit);
		FSConfig& SetBurnChance(uint8_t elemSpread0);
		FSConfig& SetSinkChance(uint8_t sinkChance0);
		FSConfig& SetParticleSpread(uint8_t spread0);
		FSConfig& SetParticleColor(chili::Color color);

		struct Defaults {
			static constexpr uint16_t MaxParticleTypes = 64;
			static constexpr uint16_t UpdateLimit = 120;
			static constexpr float GameSpeed = 1.0f;
			static constexpr uint8_t BurnChance = 0;
			static constexpr uint8_t ParticleSpread = 0;
			static constexpr uint8_t SinkChance = 0;
			static constexpr chili::Color ParticleColor = chili::Color(255, 255, 255);
		};
		void GenerateParticleMap();
		std::string GetPrettySettings() const;
		/* Variables */
	private:
		// I DON'T WANT THIS ON THE HEAP
		ParticleData types[Defaults::MaxParticleTypes];
		std::unordered_map<std::string, size_t> particleNameMap;
		uint16_t typeCnt;
		float gameSpeed;
		// this is important as, from my observations, a falling sand runs best at 120 updates/per frame
		uint16_t updateLimit;
		uint8_t burnChance;
		uint8_t particleSpread;
		uint8_t sinkChance;
		chili::Color particleColor; 
	};
}
