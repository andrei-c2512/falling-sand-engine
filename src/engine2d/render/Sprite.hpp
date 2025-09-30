#pragma once
#include "chili/Colors.hpp"
#include <string>
#include "engine2d/math/Rect.hpp"
#include <vector>

namespace e2d {
	class Sprite {
	public:
		Sprite() = default;
		Sprite(int width0, int height0);
		Sprite(const std::string& filename);
		Sprite(const Size<short>& dim, chili::Color c);
		Sprite(const Size<short>& dim, const std::vector<chili::Color>& pcolor);
		void PutPixel(int x, int y, chili::Color c);
		chili::Color GetPixel(int x, int y) const;
		void PutPixel(size_t index, chili::Color c)
		{
			pColor[index] = c;
		}
		void ScaleSprite(float scale);
		int GetWidth() const;
		int GetHeight() const;
		Rect GetRect() const;
		void Merge(const Sprite& sprite);
		void Merge(const chili::Color* sprite, int width0, int height0);
		bool WithBloom() const
		{
			return Bloom;
		}
		void Fill(chili::Color c)
		{
			for (auto& n : pColor)
			{
				n = c;
			}
		}
		chili::Color* Data() 
		{
			return pColor.data();
		}

		const chili::Color* Data() const 
		{
			return pColor.data();
		}
	public:
		std::vector<chili::Color> pColor;
		bool Bloom = false;
		int width = 0;
		int height = 0;
	};
}