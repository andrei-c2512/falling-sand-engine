#pragma once
#include <string>


#include "engine2d/math/Rect.hpp"
#include "engine2d/math/Vec2D.hpp"
#include "engine2d/render/Sprite.hpp"

namespace e2d {
	class Font {
	public:
		Font(std::string sprite) {
			LetterDim.width = SpriteSheet.GetWidth() / Columns;
			LetterDim.height = SpriteSheet.GetHeight() / Rows;

			const size_t Limit = (LastLetter - FirstLetter);
			int StartX = 0;
			int StartY = 0 - LetterDim.height;
			for (size_t ind = 0; ind < Limit; ind++)
			{
				if (ind % Columns == 0)
				{
					StartX = 0;

					StartY += LetterDim.height;
				}
				else
					StartX += LetterDim.width;


				Letters.emplace_back(RectI(LetterDim.width, LetterDim.height, Vec2I(StartX, StartY)));
			}
		}
		static constexpr char FirstLetter = ' ';
		static constexpr char LastLetter = '~';

		static constexpr int Columns = 32;
		static constexpr int Rows = 3;

		const RectI& GetLetterRect(int index) const {
			return Letters[index];
		}
		Size<int> GetLetterDim() const {
			return LetterDim;
		}
		const Sprite& GetSpriteSheet() const {
			return SpriteSheet;
		}
	private:
		Size<int> LetterDim = { 0 , 0};
		Sprite SpriteSheet;
		std::vector<RectI> Letters;
	};
}