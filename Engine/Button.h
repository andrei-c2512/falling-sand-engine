#pragma once
#include "Rect.h"
#include "Graphics.h"
#include "Sprite.h"
#include "Element.h"
#include "World.h"
#include "Dimensions.h"
#include "Weather.h"
#include "Camera.h"

class Button {
public:
	Button(RectI& rect);
	Button(RectI& rect, Color& c);
	Button(RectI& rect, Sprite& sprite);
	virtual void Draw(Graphics& gfx , Camera& ct);
	virtual void SelectSprite(){};
	bool         IsPressed(Mouse& mouse) const;
	bool         IsHovered(Mouse& mouse) const;
protected:
	bool Selected;
	RectI HitBox;
	Sprite sprite;
	Color c;

};

class ElementButton : public Button
{
public:
	static constexpr int dim = 20;
	ElementButton( Vec2I& pos , Type type0)
		:Button(RectI(dim, dim, pos) , c) , type(type0)
	{
		auto dimensions = Dimensions<short>(dim, dim);
		switch (type) {
		case Type::Water:
			sprite = Sprite(dimensions, Colors::Cyan);
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
	Type GetType() const;
public:
	void SelectSprite() override;
private:
	Type type = Type::Empty;
};

class WeatherButton : public Button
{
public:
	static constexpr short dim = 20;
	WeatherButton( Vec2I& pos,WeatherType type0)
		:Button( RectI(dim , dim , pos)) , type(type0)
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
	void        SelectSprite() override;
	WeatherType GetType() const;
private:
	WeatherType type = WeatherType::Clear;
};

class GameSpeedButtonV1 : public Button
{
public:
	GameSpeedButtonV1( const Color& default, const Color& filler ,RectI& rect)
		:Button(rect) , default_color(default) , filler_color(filler)
	{
		MaxSpeed = 5.0f;
		MinSpeed = 0.0f;
		

		IncreaseFactor = (MaxSpeed - MinSpeed) / HitBox.height;
		last_pressY = (BaseSpeed / IncreaseFactor);
		BaseY = last_pressY;
		Speed = 1.0f;
	}
	void  Update(Mouse& mouse);
	void  DetermineSpeed();
	void  Draw(Graphics& gfx, Camera& ct) override;
	void  Go(Mouse& mouse);
	float GetSpeed() const;

private:
	float MaxSpeed = 5.0f;
	float MinSpeed = 0.0f;
	static constexpr float BaseSpeed = 1.0f;
	
	float Speed = 1.0f;
	float IncreaseFactor = 0.1f;
	Color default_color = Colors::Gray;
	Color filler_color  = Colors::White;

	int BaseY;
	int last_pressY;
};

class GameSpeedButton : public Button {
public:
	GameSpeedButton(Button& button, float Speed0)
		:Speed(Speed0) , Button(button)
	{}
	void Draw(Graphics& gfx, Camera& ct) override;
	float GetSpeed()const { return Speed; };
private:
	float Speed;
};