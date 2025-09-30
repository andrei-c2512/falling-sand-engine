#include "falling-sand/ui/ElementButton.hpp"

namespace sand {
	// this is so trash gang
	ElementButton::ElementButton(const e2d::Vec2I& pos, Type type0)
		:Button(e2d::RectI(dim, dim, pos), c), type(type0)
	{
		auto dimensions = e2d::Size<short>(dim, dim);
		switch (type) {
		case Type::Water:
			sprite = e2d::Sprite(dimensions, chili::Colors::Cyan);
			break;
		case Type::Sand:
			sprite = e2d::Sprite(dimensions, chili::Colors::Yellow);
			break;
		case Type::Stone:
			sprite = e2d::Sprite(dimensions, chili::Colors::LightGray);
			break;
		case Type::Fire:
			sprite = e2d::Sprite(dimensions, chili::Colors::Red);
			break;
		case Type::Wood:
			sprite = e2d::Sprite(dimensions, chili::Color(165, 80, 0));
			break;
		case Type::Smoke:
			sprite = e2d::Sprite(dimensions, chili::Color(55, 55, 55));
			break;
		case Type::ToxicGas:
			sprite = e2d::Sprite(dimensions, chili::Color(0, 100, 0));
			break;
		case Type::Acid:
			sprite = e2d::Sprite(dimensions, chili::Color(0, 135, 0));
			break;
		case Type::Snow:
			sprite = e2d::Sprite(dimensions, chili::Color(240, 240, 255));
			break;
		default:
			sprite = e2d::Sprite(dimensions, chili::Color(255, 255, 255));
			break;
		}
	}
	void ElementButton::SelectSprite()
	{
		auto dimensions = e2d::Size<short>(dim, dim);
		
		switch (type) {
		case Type::Water:
			sprite = e2d::Sprite(dimensions, chili::Colors::Cyan);
			break;
		case Type::Sand:
			sprite = e2d::Sprite(dimensions, chili::Colors::Yellow);
			break;
		case Type::Stone:
			sprite = e2d::Sprite(dimensions, chili::Colors::LightGray);
			break;
		case Type::Fire:
			sprite = e2d::Sprite(dimensions, chili::Colors::Red);
			break;
		case Type::Wood:
			sprite = e2d::Sprite(dimensions, chili::Color(165, 80, 0));
			break;
		case Type::Smoke:
			sprite = e2d::Sprite(dimensions, chili::Color(55, 55, 55));
			break;
		case Type::ToxicGas:
			sprite = e2d::Sprite(dimensions, chili::Color(0, 100, 0));
			break;
		case Type::Acid:
			sprite = e2d::Sprite(dimensions, chili::Color(0, 135, 0));
			break;
		case Type::Snow:
			sprite = e2d::Sprite(dimensions, chili::Color(240, 240, 255));
			break;
		default:
			sprite = e2d::Sprite(dimensions, chili::Color(255, 255, 255));
			break;
		}
	}

	Type ElementButton::GetType() const {
		return type;
	}
}


