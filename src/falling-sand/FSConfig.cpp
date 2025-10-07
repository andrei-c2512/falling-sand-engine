#include "falling-sand/FSConfig.hpp"
#include <assert.h>

namespace sand {

	void ParticleData::AddColor(chili::Color color) {
		assert(colorCnt != MaxColorRange);
		colorRange[colorCnt] = color;
		colorCnt++;
	
	}
	// needs optimising
	const ParticleData& FSConfig::GetParticle(const std::string_view name) const {
		auto it = particleNameMap.find(std::string(name));
		if (it == particleNameMap.end()) {
			throw std::invalid_argument("Inserted invalid particle name");
		}
		return types[it->second];
	}
	const ParticleData& FSConfig::GetParticle(const int8_t id) const{
		return types[id];
	}

	void FSConfig::GenerateParticleMap() {
		for (size_t i = 0; i < typeCnt; i++) {
			particleNameMap[types[i].name] = i;
		}
	}

	int FSConfig::TypeCount() const {
		return typeCnt;
	}

	FSConfig& FSConfig::SetParticleTypes(const std::vector<ParticleData>& typeList) {
		typeCnt = typeList.size();

		for (size_t i = 0; i < typeCnt; i++) {
			types[i] = typeList[i];
		}
		return *this;
	}
	FSConfig& FSConfig::SetSpeed(float speed) {
		gameSpeed = speed;
		return *this;
	}
	FSConfig& FSConfig::SetUpdateLimit(uint8_t limit) {
		updateLimit = limit;
		return *this;
	}
	FSConfig& FSConfig::SetBurnChance(uint8_t elemSpread0) {
		particleSpread = elemSpread0;
		return *this;
	}
	FSConfig& FSConfig::SetSinkChance(uint8_t sinkChance0) {
		sinkChance = sinkChance0;
		return *this;
	}
	FSConfig& FSConfig::SetParticleSpread(uint8_t spread0) {
		particleSpread = spread0;
		return *this;
	}
	FSConfig& FSConfig::SetParticleColor(chili::Color color) {
		particleColor = color;
		return *this;
	}
	void FSConfig::AddParticleType(const ParticleData& data) {
		assert(typeCnt != Defaults::MaxParticleTypes);
		types[typeCnt] = data;
	}


	std::string FSConfig::GetPrettySettings() const {
		return "";
	}
}