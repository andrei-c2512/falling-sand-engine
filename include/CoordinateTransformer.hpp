#pragma once
#include "Graphics.hpp"
#include "Mouse.hpp"
class CoordinateTransformer {
public:
	CoordinateTransformer(Graphics& gfx0)
		:gfx(gfx0)
	{

	}
	template<typename T>
	Vec2_<T> Transform(Vec2_<T> pos) const
	{
		pos.y -= Graphics::ScreenHeight;
		pos.y = -pos.y;
		return pos;
	}
	template<typename T>
	Vec2_<T> Transform(T x, T y) const
	{
		// reversing the y axis and making the bottom left the { 0 , 0 } point
		y -= Graphics::ScreenHeight;
		y = -y;
		return Vec2_<T>(x, y);
	}
	int TransformX(int x) const 
	{
		return x + Graphics::ScreenWidth / 2;
	}
	int TransformY(int y) const
	{
		return -y + Graphics::ScreenHeight / 2;
	}
	static Vec2I TransformMousePos(const Mouse& mouse)
	{
		Vec2I pos = mouse.GetPos();
		pos.y -= Graphics::ScreenHeight;
		pos.y = -pos.y;
		return pos;

	}
	static Vec2I TransformScrPos(Vec2I pos) 
	{
		pos.y -= Graphics::ScreenHeight;
		pos.y = -pos.y;
		return pos;
	}
private:
	Graphics& gfx;
};