#pragma once
#include "Camera.h"
#include "Mouse.h"
#include "Font.h"
#include <string>

class CoordinateShower {
public:
	CoordinateShower(Camera& cam0)
		:cam(cam0)
	{}
	void ShowCoordinates(Graphics& gfx, Mouse& mouse)
	{
		auto coordinates = DetermineCoordinates(mouse);
		std::string coords = "X: " + std::to_string(coordinates.x) + "  Y: " + std::to_string(coordinates.y);
		font.DrawWords(coords , gfx , cam , Vec2I(Graphics::ScreenWidth - coords.length() * font.GetLetterDim().width , 
												  font.GetLetterDim().height));
	}
	Vec2I DetermineCoordinates(Mouse& mouse)
	{
		Vec2I mouse_pos = mouse.GetPos();
		mouse_pos.y = Graphics::ScreenHeight - mouse_pos.y;
		Vec2I world_pos = mouse_pos + cam.GetPos();
		return world_pos;
	}
	static Vec2I DetermineCoordinates(Mouse& mouse, Camera& camera)
	{
		Vec2I mouse_pos = mouse.GetPos();
		mouse_pos.y = Graphics::ScreenHeight - mouse_pos.y;
		Vec2I world_pos = mouse_pos + camera.GetPos();
		return world_pos;
	}
	static Vec2I DetermineCoordinates(Vec2I point, Camera& camera)
	{
		point.y = Graphics::ScreenHeight - point.y;
		Vec2I world_pos = point + camera.GetPos();
		return world_pos;
	}
private:
	Camera& cam;
	Font font = { "assets/Fixedsys16x28.bmp" };
};