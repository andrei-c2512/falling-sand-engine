#include "Weather.hpp"

void Weather::UpdateMatrix()
{
	if(type != WeatherType::Clear)
	{
		Type ElemType = Type::Empty;

		switch (type)
		{
		case WeatherType::Rain:
			ElemType = Type::Water;
			break;
		case WeatherType::Snowy:
			ElemType = Type::Snow;
			break;
		default:
			break;
		}

		if (ElemType != Type::Empty)
		{
			for (size_t ind = 0; ind < ParticleLimit; ind++)
			{
				size_t index = ParticleSpread.GetVal();

				world.AddToSpawnList(World::Spawn(index,  ElemType));
			}
		}
	}
}


void Weather::SetWeather(WeatherType type0)
{
	type = type0;
}