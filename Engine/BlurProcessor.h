#pragma once
#include "Sprite.h"
#include <vector>
#include "Dimensions.h"
#include "CiobanuMath.h"
#include <algorithm>
#include <functional>

class BlurProcessor {
public:
	BlurProcessor(Sprite& sprite)
		:bloomed_sprite(sprite)
	{
		float* kernelFloat = new float[diameter];
		
		for (int x = 0; x < diameter; x++)
		{
			kernelFloat[x] = gaussian(float(std::abs(x - diameter / 2)), float(diameter / 6.0f));
		}

		for (int x = 0; x < diameter; x++)
		{
			kernel[x] = (255 * (kernelFloat[x] / kernelFloat[diameter / 2]));
		}
		for (int x = 0; x < diameter; x++)
		{
			sumKernel += kernel[x];
		}

		sumKernel = unsigned int(sumKernel / OverDriveFactor);
		delete[] kernelFloat;
	}
	void DownSize()
	{
		int new_width = bloomed_sprite.GetWidth() / ScaleSize;
		int new_height = bloomed_sprite.GetHeight() / ScaleSize;
		
		std::vector<Color> pColor;
		pColor.resize(new_width *  new_height);
		
		for (size_t y = 0; y < bloomed_sprite.GetHeight(); y += ScaleSize)
		{
			for (size_t x = 0; x < bloomed_sprite.GetWidth();x += ScaleSize)
			{
		
				unsigned int r, g, b;
				r = g = b = 0u;
		
				// blending a ScaleSize x ScaleSize rect
				for (size_t y0 = y; y0 < y + ScaleSize; y0++)
				{
					for (size_t x0 = x; x0 < x + ScaleSize; x0++)
					{
						const Color c = bloomed_sprite.GetPixel(x0, y0);
						r += c.GetR();
						g += c.GetG();
						b += c.GetB();
					}
				}
				auto Area = ScaleSize * ScaleSize;
				r /= Area;
				g /= Area;
				b /= Area;
		
				const Color new_color = Color(r, g, b);
		
				pColor[(y / ScaleSize) * new_width + x / ScaleSize] = new_color;
			}
		}
		downBuffer = Sprite(Dimensions<int>(new_width, new_height) , pColor);
	}
	void UpSize()
	{
		int new_width = downBuffer.GetWidth() * ScaleSize;
		int new_height = downBuffer.GetHeight() * ScaleSize;
		
		std::vector<Color> pColor;
		pColor.resize(new_width * new_height);
		
		for (size_t y = 0; y < downBuffer.GetHeight(); y += 1)
		{
			for (size_t x = 0; x < downBuffer.GetWidth(); x += 1)
			{
				// filling a ScaleSize x ScaleSize rect
				for (size_t y0 = y * ScaleSize; y0 < y * ScaleSize + ScaleSize; y0++)
				{
					for (size_t x0 = x * ScaleSize; x0 < x * ScaleSize + ScaleSize; x0++)
					{
						const Color c = downBuffer.GetPixel(x, y);
						pColor[(y0 ) * new_width + x0 ] = c;
					}
				}
			}
		}
		bloomed_sprite = Sprite(Dimensions<int>(new_width, new_height), pColor);
	}
	void HorizontalPass()
	{
		Color* pColor = downBuffer.Data();
		for (size_t y = 0; y < downBuffer.GetHeight(); y += 1)
		{
			size_t index_height = y * downBuffer.GetWidth();

			for (size_t x = 0; x < downBuffer.GetWidth() - diameter / 2; x += 1)
			{
				for (int i = 0; i < diameter; i++)
				{
					Color& c = pColor[index_height + x + i];
					c = Color(minimum(unsigned int(c.GetR() * (kernel[i] + 1) / sumKernel),  255u),
							  minimum(unsigned int(c.GetG() * (kernel[i] + 1) / sumKernel),  255u),
							  minimum(unsigned int(c.GetB() * (kernel[i] + 1) / sumKernel),  255u) );
				}
			}
		}
	}
	void VerticalPass()
	{
		Color* pColor = downBuffer.Data();
		for (size_t y = 0; y < downBuffer.GetHeight() - diameter / 2; y += 1)
		{
			size_t index_height = y * downBuffer.GetWidth();

			for (size_t x = 0; x < downBuffer.GetWidth(); x += 1)
			{
				for (int i = 0; i < diameter; i++)
				{
					Color& c = pColor[index_height + x + i * downBuffer.GetWidth()];
					c = Color(minimum(unsigned int(c.GetR() * (kernel[i] + 1) / sumKernel), 255u),
						      minimum(unsigned int(c.GetG() * (kernel[i] + 1) / sumKernel), 255u),
						      minimum(unsigned int(c.GetB() * (kernel[i] + 1) / sumKernel), 255u));
				}
			}
		}
	}
	void Go()
	{
		DownSize();
		//HorizontalPass();
		//VerticalPass();
		UpSize();
	}
private:
	unsigned int ScaleSize = 10u;
	static constexpr int diameter = 8;
	unsigned int kernel[diameter];
	unsigned int sumKernel = 0;
	unsigned int OverDriveFactor = 2.0f;
	std::function<int(int, int)> minimum = [](int x1 , int x2) {
		if (x2 > x1)
		{
			return x1;
		}
		else
			return x2;
	};
	Sprite& bloomed_sprite;
	Sprite downBuffer;
};