#pragma once
#include "engine2d/core/Graphics.hpp"
#include "chili/Mouse.hpp"
#include "engine2d/math/Vec2D.hpp"
#include "engine2d/core/MouseAPI.hpp"

namespace e2d {
	/*
	* This class still depends on chili::Graphics. This needs to be changed in the future 
	* to something like "WindowSettings" or sum
	*/
	//template<MouseAPI Mouse>
	class CoordinateTransformer {
	public:
		CoordinateTransformer(e2d::Renderer& gfx0)
			:gfx(gfx0)
		{
		}
		template<typename T>
		Vec2_<T> Transform(Vec2_<T> pos) const
		{
			pos.y -= Renderer::ScreenHeight;
			pos.y = -pos.y;
			return pos;
		}
		template<typename T>
		Vec2_<T> Transform(T x, T y) const
		{
			// reversing the y axis and making the bottom left the { 0 , 0 } point
			y -= Renderer::ScreenHeight;
			y = -y;
			return Vec2_<T>(x, y);
		}
		int TransformX(int x) const
		{
			return x + chili::Graphics::ScreenWidth / 2;
		}
		int TransformY(int y) const
		{
			return -y + chili::Graphics::ScreenHeight / 2;
		}
		static Vec2I TransformMousePos(const chili::Mouse& mouse)
		{
			Vec2I pos = mouse.GetPos();
			pos.y -= chili::Graphics::ScreenHeight;
			pos.y = -pos.y;
			return pos;

		}
		static Vec2I TransformScrPos(Vec2I pos)
		{
			pos.y -= chili::Graphics::ScreenHeight;
			pos.y = -pos.y;
			return pos;
		}
	private:
		const e2d::Renderer& gfx;
	};
}