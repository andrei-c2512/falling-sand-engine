#include "Sprite.h"
#include <fstream>
#include <assert.h>
#include "ChiliWin.h"
#include <algorithm>
#include <iterator>

Sprite::Sprite(int width0, int height0)
	:width(width0), height(height0)
{
	pColor.resize(width * height);
}
Sprite::Sprite(const Dimensions<short>& dim, Color c)
{
	width = dim.width; height = dim.height;
	pColor.resize(width * height);
	for (int i = 0 ; i < width * height ; i++)
	{
		pColor[i] = c;
	}
}
Sprite::Sprite(const std::string& filename)
{
	std::ifstream file(filename , std::ios::binary);
	assert(file);
	
	BITMAPFILEHEADER bmFILEHEADER;
	file.read(reinterpret_cast<char*>(&bmFILEHEADER), sizeof(bmFILEHEADER));
	
	BITMAPINFOHEADER bmINFOHEADER;
	file.read(reinterpret_cast<char*>(&bmINFOHEADER), sizeof(bmINFOHEADER));
	
	//assert(bmINFOHEADER.biCompression == BI_RGB);
	assert(bmINFOHEADER.biBitCount == 24 || bmINFOHEADER.biBitCount == 32);
	
	width = bmINFOHEADER.biWidth;
	height = bmINFOHEADER.biHeight;
	
	int StartY, EndY , dy;
	if (height > 0)
	{
		StartY = height - 1;
		EndY = -1;
		dy = -1;
	}
	else
	{
		height = std::abs(height);
		StartY = 0;
		EndY = height - 1;
		dy = 1;
	}
	pColor.resize(width * height);
	file.seekg(bmFILEHEADER.bfOffBits);
	
	const int Padding = (4 - (width * bmINFOHEADER.biCompression % 4)) % 4;
	
	bool is32bit = bmINFOHEADER.biBitCount == 32;

	for (int y = StartY; y != EndY; y += dy)
	{
		for (int x = 0; x < width; x++)
		{
			PutPixel(x, y, Color(file.get(), file.get(), file.get()));
			if (is32bit)
			{
				file.seekg(1, std::ios::cur);
			}
		}
		if (!is32bit)
		{
			file.seekg(Padding, std::ios::cur);
		}
	}


}
int Sprite::GetWidth() const {
	return width;
}

void Sprite::ScaleSprite(float scale)
{
	Sprite old = *this;

	width *= scale;
	height *= scale;
	pColor.resize(width * height);
	for (float y = 0; y < height; y += scale)
	{
		for (float x = 0; x < width; x += scale)
		{
			Color c = old.GetPixel(x / scale, y / scale);

			for (float y0 = y; y0 < y + scale; y0 += 1)
			{
				for (float x0 = x; x0 <  x + scale; x0 += 1)
				{
					pColor[int(y0 * width + x0)] = c;
				}
			}
		}
	}
}
int Sprite::GetHeight() const {
	return height;
}

void Sprite::PutPixel(int x, int y, Color c)
{
	pColor[(y * width) + x] = c;
}

Color Sprite::GetPixel(int x, int y)
{
	return pColor[y * width + x];
}

Rect Sprite::GetRect() const {
	return Rect(width, height, Vec2D(0.0f, 0.0f));
}

Sprite::Sprite(const Dimensions<short>& dim, std::vector<Color>& pcolor)
{
	pColor = pcolor;
	width = dim.width;
	height = dim.height;
}
void Sprite::Merge(const Sprite& sprite)
{
	assert(width == sprite.width && height == sprite.height);

	for (size_t ind = 0; ind < width * height; ind++)
	{
		int r, g, b;
		r = g = b = 0;

		r = pColor[ind].GetR() + sprite.pColor[ind].GetR();
		if (r > 255)
		{
			r = 255;
		}
		g = pColor[ind].GetG() + sprite.pColor[ind].GetG();
		if (g > 255)
		{
			g = 255;
		}
		b = pColor[ind].GetB() + sprite.pColor[ind].GetB();
		if (r > 255)
		{
			b = 255;
		}

		pColor[ind] = Color(r, g, b);
	}
}

void Sprite::Merge(const Color* sprite, int width0, int height0)
{
	assert(width == width0 && height == height0);

	for (size_t ind = 0; ind < width * height; ind++)
	{
		int r, g, b;
		r = g = b = 0;

		r = pColor[ind].GetR() + pColor[ind].GetR();
		if (r > 255)
		{
			r = 255;
		}
		g = pColor[ind].GetG() + pColor[ind].GetG();
		if (g > 255)
		{
			g = 255;
		}
		b = pColor[ind].GetB() + pColor[ind].GetB();
		if (r > 255)
		{
			b = 255;
		}

		pColor[ind] = Color(r, g, b);
	}
}