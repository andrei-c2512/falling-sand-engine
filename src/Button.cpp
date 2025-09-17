#include "Button.hpp"
#include "Effects.hpp"
Button::Button(const RectI& rect)
	:HitBox(rect) 
{
	Selected = false;
}
Button::Button(const RectI& rect, Color c)
	:HitBox(rect) , sprite(HitBox.GetDimensions() , c)
{
	Selected = false;
}

Button::Button(const RectI& rect, const Sprite& sprite0)
	: HitBox(rect), sprite(sprite0)
{
	Selected = false;

}
void Button::Draw(Graphics& gfx, Camera& cam)
{
	Effects::Copy e;
	Vec2I button_pos = cam.ToCamera(HitBox.GetPos());
	gfx.DrawSprite(std::move(button_pos), sprite, RectI(HitBox.width, HitBox.height, Vec2I(0, 0))
		, Graphics::GetScreenRect(), e);
}

void ElementButton::SelectSprite()
{
	auto dimensions = Dimensions<short>(dim, dim);
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
		sprite = Sprite(dimensions, Color( 165 , 80  , 0 ));
		break;
	case Type::Smoke:
		sprite = Sprite(dimensions, Color( 55  , 55  , 55 ));
		break;
	case Type::ToxicGas:
		sprite = Sprite(dimensions, Color( 0 , 100  , 0 ));
		break;
	case Type::Acid:
		sprite = Sprite(dimensions, Color( 0  , 135  , 0 ));
		break;
	case Type::Snow:
		sprite = Sprite(dimensions, Color( 240  , 240  , 255 ));
		break;
	default:
		sprite = Sprite(dimensions, Color( 255  , 255  , 255 ));
		break;
	}
}

void WeatherButton::SelectSprite()
{
	auto dimensions = Dimensions<short>(dim, dim);
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
		if (HitBox.PointInRect(CoordinateTransformer::TransformMousePos(mouse)))
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
	return HitBox.PointInRect(CoordinateTransformer::TransformMousePos(mouse));
}

void GameSpeedButtonV1::Update(Mouse& mouse)
{
	assert(IsPressed(mouse));
	auto MouseY = mouse.GetPosY();
	
	last_pressY = HitBox.top() - MouseY;
	DetermineSpeed();

}

void GameSpeedButtonV1::DetermineSpeed()
{
	//distance from the center , where the default value is established
	int dist =  last_pressY - BaseY ;

	float add = float(dist) * IncreaseFactor;

	Speed = BaseSpeed + add;
	if (Speed < 0.0f)
	{
		Speed = 0.0f;
	}
}

float GameSpeedButtonV1::GetSpeed() const {
	return Speed;
}

void GameSpeedButtonV1::Go(Mouse& mouse) {
	DetermineSpeed();
	if (IsPressed(mouse))
	{
		Update(mouse);
	}
}

void GameSpeedButtonV1::Draw(Graphics& gfx, Camera& cam)
{
	//pos of the unfilled part of the bar
	Vec2I unfilled_pos = cam.ToCamera(HitBox.GetPos());
	gfx.DrawRect(RectI(HitBox.width, HitBox.height - last_pressY, Vec2I(std::move(unfilled_pos))),
		default_color , Effects::Copy{});

	//pos of the filled part of the bar
	Vec2I filled_pos = cam.ToCamera(Vec2I(HitBox.left , HitBox.bottom + (HitBox.height - last_pressY)));
	gfx.DrawRect(RectI(HitBox.width, last_pressY , Vec2I(std::move(filled_pos))),
		filler_color, Effects::Copy{});
}

void GameSpeedButton::Draw(Graphics& gfx, Camera& cam)
{
	Effects::Chroma chroma = { Colors::Magenta };
	Vec2I pos = cam.ToCamera(Vec2I(HitBox.GetPos()));
	gfx.DrawSprite(std::move(pos), sprite, sprite.GetRect(), Graphics::GetScreenRect(), chroma);
}