#include "engine2d/ui/Button.hpp"

#include "falling-sand/Weather.hpp"

namespace sand {
	class WeatherButton : public e2d::Button
	{
	public:
		static constexpr short dim = 20;
		WeatherButton(const e2d::Vec2I& pos, WeatherType type0)
			:Button(e2d::RectI(dim, dim, pos)), type(type0)
		{
			auto dimensions = e2d::Size<short>(dim, dim);

			switch (type) {
			case WeatherType::Clear:
				sprite = e2d::Sprite(dimensions, chili::Colors::Yellow);
				break;
			case WeatherType::Rain:
				sprite = { "assets/Weather_rain_button20x20.bmp" };
				break;
			case WeatherType::Snowy:
				sprite = e2d::Sprite(dimensions, chili::Colors::White);
				break;
			default:
				break;
			}
		}
		void        SelectSprite() ;
		WeatherType GetType() const;
	private:
		WeatherType type = WeatherType::Clear;
	};
}
