#include "Font.hpp"
#include "assert.h"
#include "Effects.hpp"

Font::Font(std::string sprite)
	:SpriteSheet(sprite)
{
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


void Font::DrawWords(std::string text, Graphics& gfx , Camera& ct , Vec2I pos)
{
	for (auto& c : text)
	{
		if (c >= FirstLetter && c <= LastLetter)
		{
			const size_t index = c - FirstLetter;
			RectI Letter = Letters[index];

			Effects::OneColor e{ Colors::White , Colors::White};
			gfx.DrawSprite(ct.ToCamera(pos), SpriteSheet, Letter, Graphics::GetScreenRect(), e);
			pos.x += LetterDim.width;
		}
	}
}

Dimensions<int> Font::GetLetterDim() const {
	return LetterDim;
}