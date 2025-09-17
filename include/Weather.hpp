#pragma once
#include "World.hpp"
enum class WeatherType {
	Clear,
	Rain,
	Snowy,
	Count,
};
class Weather {
public:
	Weather(World& world0 , WeatherType weather , size_t limit)
		:world(world0) , type(weather),ParticleSpread(0 , world.GetSandboxDim().width - 1)
	{
		ParticleLimit = limit;
	}
	void UpdateMatrix();
	void SetWeather(WeatherType weather);
private:
	WeatherType type;
	World& world ;

	size_t ParticleLimit;

	RNG ParticleSpread;
};