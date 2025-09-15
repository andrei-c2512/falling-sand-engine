#pragma once
#include "Colors.h"
#include <string>
#include "Rect.h"
#include "Vec2D.h"
#include "Dimensions.h"
#include <vector>

class Sprite {
public:
	Sprite() = default;
	Sprite(int width0, int height0);
	Sprite(const std::string& filename);
	Sprite(const Dimensions<short>& dim, Color c);
	Sprite(const Dimensions<short>& dim, const std::vector<Color>& pcolor);
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	void PutPixel(size_t index , Color c)  
	{
		pColor[index] = c;
	}
	void ScaleSprite(float scale);
	int GetWidth() const;
	int GetHeight() const;
	Rect GetRect() const;
	void Merge(const Sprite& sprite);
	void Merge(const Color* sprite, int width0, int height0);
	bool WithBloom() const
	{
		return Bloom;
	}
	void Fill(Color c)
	{
		for (auto& n : pColor)
		{
			n = c;
		}
	}
	Color* Data()
	{
		return pColor.data();
	}
	RectI GetRect()
	{
		return RectI(width, height, Vec2I(0, 0));
	}
public:
	std::vector<Color> pColor;
	bool Bloom = false;
	int width = 0;
	int height = 0;
};