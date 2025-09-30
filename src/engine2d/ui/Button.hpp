#pragma once
#include "engine2d/math/Rect.hpp"
#include "chili/Colors.hpp"
#include "engine2d/core/GraphicsAPI.hpp"
#include "engine2d/core/MouseAPI.hpp"
#include "engine2d/pipeline/Camera.hpp"
#include "engine2d/render/Sprite.hpp"
#include "chili/Mouse.hpp"

namespace e2d {
	// template<GraphicsAPI E2DRenderer, MouseAPI Mouse>
	class Button {
	public:
		Button(const RectI& rect) 
			:HitBox(rect)
		{
			Selected = false;
		}
		Button(const RectI& rect, chili::Color c) 
			:HitBox(rect) , sprite(HitBox.GetSize() , c)
		{
			Selected = false;
		}
		Button(const RectI& rect, const Sprite& sprite0) 
			: HitBox(rect), sprite(sprite0)
		{
			Selected = false;
		}
		virtual void Draw(Renderer& renderer, const Camera& cam) {
			Vec2I button_pos = cam.ToCamera(HitBox.GetPos());
			renderer.DrawSprite(button_pos.x, button_pos.y, sprite, e2d::RectI(HitBox.width, HitBox.height, Vec2I(0, 0)), Renderer::GetScreenRect());
		}
		bool IsPressed(const chili::Mouse& mouse) const {
			Vec2I MousePos = mouse.GetPos();
			if(mouse.LeftIsPressed())
			{
				if (HitBox.PointInRect(CoordinateTransformer::TransformMousePos(mouse)))
				{
					return true;
				}
			}
			return false;
		}
		bool IsHovered(const chili::Mouse& mouse) const {
			return HitBox.PointInRect(CoordinateTransformer::TransformMousePos(mouse));
		}
	protected:
		bool Selected;
		RectI HitBox;
		Sprite sprite;
		chili::Color c;
	};
}



/*
class GameSpeedButtonV1 : public Button
{
public:
	GameSpeedButtonV1( const Color& def, const Color& filler ,const RectI& rect)
		:Button(rect) , default_color(def) , filler_color(filler)
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
}*/
