#include "Button.h"
#include "Effects.h"
Button::Button(RectI& rect)
	:HitBox(rect)
{
	Selected = false;
}
Button::Button(RectI& rect, Color& c)
	:HitBox(rect) , sprite(HitBox.GetDimensions() , c)
{
	Selected = false;
}

Button::Button(RectI& rect, Sprite& sprite0)
	: HitBox(rect), sprite(sprite0)
{
	Selected = false;

}
void Button::Draw(Graphics& gfx)
{
	Effects::Copy e;
	gfx.DrawSprite(HitBox.left, HitBox.top, sprite, RectI(HitBox.width, HitBox.height, Vec2I(0, 0)) 
		, Graphics::GetScreenRect(), e);
}

void ElementButton::SelectSprite()
{
	auto dimensions = Dimensions<int>(dim, dim);
	switch (type) {
	case Type::Water:
		sprite = Sprite(dimensions, Colors::Cyan );
		break;
	case Type::Sand:
		sprite = Sprite(dimensions, Colors::Yellow);
		break;
	case Type::Stone:
		sprite = Sprite(dimensions, Colors::LightGray);
		break;
	case Type::Fire:
		sprite = Sprite(dimensions, Colors::Red);
		break;
	case Type::Wood:
		sprite = Sprite(dimensions, { 165 , 80  , 0 });
		break;
	case Type::Smoke:
		sprite = Sprite(dimensions, { 55  , 55  , 55 });
		break;
	case Type::ToxicGas:
		sprite = Sprite(dimensions, { 0 , 100  , 0 });
		break;
	case Type::Acid:
		sprite = Sprite(dimensions, { 0  , 135  , 0 });
		break;
	case Type::Snow:
		sprite = Sprite(dimensions, { 240  , 240  , 255 });
		break;
	default:
		sprite = Sprite(dimensions, { 255  , 255  , 255 });
		break;
	}
}

void WeatherButton::SelectSprite()
{
	auto dimensions = Dimensions<int>(dim, dim);
	switch (type) {
	case WeatherType::Clear:
		sprite = Sprite(dimensions, Colors::Yellow);
		break;
	case WeatherType::Rain:
		sprite = { "Weather_rain_button20x20.bmp" };
		break;
	case WeatherType::Snowy:
		sprite = Sprite(dimensions, Colors::White);
		break;
	default:
		break;
	}
}

bool Button::IsPressed(Mouse& mouse) const
{
	Vec2I MousePos = mouse.GetPos();

	if(mouse.LeftIsPressed())
	{
		if (HitBox.PointInRect(MousePos))
		{
			return true;
		}
	}
}

WeatherType WeatherButton::GetType() const
{
	return type;
}

Type ElementButton::GetType() const
{
	return type;
}

bool Button::IsHovered(Mouse& mouse) const
{
	return HitBox.PointInRect(mouse.GetPos());
}