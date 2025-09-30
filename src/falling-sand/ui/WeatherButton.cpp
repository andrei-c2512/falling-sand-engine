#include "falling-sand/ui/WeatherButton.hpp"

namespace sand {
	void WeatherButton::SelectSprite()
	{
		auto dimensions = e2d::Size<short>(dim, dim);
		switch (type) {
		case WeatherType::Clear:
			sprite = e2d::Sprite(dimensions, chili::Colors::Yellow);
			break;
		case WeatherType::Rain:
			sprite = { "Weather_rain_button20x20.bmp" };
			break;
		case WeatherType::Snowy:
			sprite = e2d::Sprite(dimensions, chili::Colors::White);
			break;
		default:
			break;
		}
	}

	WeatherType WeatherButton::GetType() const
	{
		return type;
	}
}
