#pragma once
#include "Graphics.h"
#include "String.h"
#include "Dimensions.h"
#include "Rect.h"
#include "Camera.h"
class Font {
public:
	Font(std::string sprite);
	void DrawWords(std::string text, Graphics& gfx , Camera& ct , Vec2I pos);
	static constexpr char FirstLetter = ' ';
	static constexpr char LastLetter = '~';

	static constexpr int Columns = 32;
	static constexpr int Rows = 3;

	Dimensions<int> GetLetterDim() const;
private:
	Dimensions<int> LetterDim = { 0 , 0};
	Sprite SpriteSheet;
	std::vector<RectI> Letters;

};