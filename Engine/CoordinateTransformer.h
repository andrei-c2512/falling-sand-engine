#pragma once
#include "Graphics.h"
class CoordinateTransformer {
public:
	CoordinateTransformer(Graphics& gfx0)
		:gfx(gfx0)
	{

	}
	template<typename T>
	Vec2_<T> Transform(Vec2_<T> pos)
	{
		pos.x += Graphics::ScreenWidth / 2;
		pos.y = -pos.y + Graphics::ScreenHeight / 2;
		return pos;
	}
	template<typename T>
	Vec2_<T> Transform(T x, T y)
	{
		x += Graphics::ScreenWidth / 2;
		y = -y + Graphics::ScreenHeight / 2;
		return Vec2_<T>(x, y);
	}
	int TransformX(int x)
	{
		return x + Graphics::ScreenWidth / 2;
	}
	int TransformY(int y)
	{
		return -y + Graphics::ScreenHeight / 2;
	}
private:
	Graphics& gfx;
};