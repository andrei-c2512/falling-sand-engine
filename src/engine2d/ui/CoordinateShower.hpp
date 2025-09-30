#pragma once
#include "engine2d/pipeline/Camera.hpp"
#include "chili/Mouse.hpp"
#include "engine2d/render/Font.hpp"
#include "engine2d/math/Vec2D.hpp"
#include "engine2d/core/GraphicsAPI.hpp"
#include "engine2d/core/MouseAPI.hpp"
#include "engine2d/core/Graphics.hpp"

#include <string>

namespace e2d {
	//template<GraphicsAPI E2DRenderer, MouseAPI Mouse>
	class CoordinateShower {
	public:
		CoordinateShower(Camera& cam0)
			:cam(cam0)
		{}
		void ShowCoordinates(e2d::Renderer& gfx,const  chili::Mouse& mouse)
		{
			auto coordinates = DetermineCoordinates(mouse);
			std::string coords = "X: " + std::to_string(coordinates.x) + "  Y: " + std::to_string(coordinates.y);
			
			gfx.DrawString(e2d::Renderer::ScreenWidth - coords.length() * font.GetLetterDim().width, font.GetLetterDim().height,
				coords.data(), font);
		}
		Vec2I DetermineCoordinates(const chili::Mouse& mouse)
		{
			Vec2I mouse_pos = mouse.GetPos();
			// modify this later
			mouse_pos.y = chili::Graphics::ScreenHeight - mouse_pos.y;
			//
			Vec2I world_pos = mouse_pos + cam.GetPos();
			return world_pos;
		}
		static Vec2I DetermineCoordinates(const chili::Mouse& mouse, const Camera& camera)
		{
			Vec2I mouse_pos = mouse.GetPos();
			mouse_pos.y = e2d::Renderer::ScreenHeight - mouse_pos.y;
			Vec2I world_pos = mouse_pos + camera.GetPos();
			return world_pos;
		}
		static Vec2I DetermineCoordinates(Vec2I point, const Camera& camera)
		{
			point.y = e2d::Renderer::ScreenHeight - point.y;
			Vec2I world_pos = point + camera.GetPos();
			return world_pos;
		}
	private:
		Camera& cam;
		Font font = { "assets/Fixedsys16x28.bmp" };
	};
}