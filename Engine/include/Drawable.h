#pragma once
#include "Graphics.h"
#include <vector>

class Drawable {
public:
	Drawable(std::vector<Vec2I>& poli)
		:model(poli)
	{

	}
	void Translate(Vec2I& trans)
	{
		Translation += trans;
	}
	void TranslateX(int x)
	{
		Translation.x += x;
	}
	void TranslateY(int y)
	{
		Translation.y += y;
	}
	void ApplyScale(float scale)
	{
		Scale_x *= scale;
		Scale_y *= scale;
		Translation.x = int(float(Translation.x) * scale);
		Translation.y = int(float(Translation.y) * scale);
	}
	void ApplyScaleX(float scale)
	{
		Scale_x = Scale_x * scale;
		Translation.x = int(float(Translation.x) * scale);
	}
	void ApplyScaleY(float scale)
	{
		Scale_y = Scale_y * scale;
		Translation.y = int(float(Translation.y) * scale);
	}
	void Render(Graphics& gfx , Color color)
	{
		for (auto& point : model)
		{
			point.x = int(float(point.x) * Scale_x);
			point.y = int(float(point.y) * Scale_y);

			point += Translation;
		}

		//gfx.DrawClosedPoly(model, color);
	}
private:
	std::vector<Vec2I> model;

	float Scale_x = 1.0f;
	float Scale_y = 1.0f;

	Vec2I Translation = { 0.0f , 0.0f };
};