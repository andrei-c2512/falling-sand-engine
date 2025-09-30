#pragma once
#include "Sprite.hpp"
#include <vector>
#include "engine2d/math/Size.hpp"
#include "engine2d/math/Math.hpp"
#include <algorithm>
#include <functional>

namespace e2d {
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
				kernel[x] = unsigned int(255 * (kernelFloat[x] / kernelFloat[diameter / 2]));
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
			downBuffer = DownSize(bloomed_sprite, ScaleSize);
		}
		static Sprite DownSize(Sprite& sprite, int scale)
		{
			int new_width = sprite.GetWidth() / scale;
			int new_height = sprite.GetHeight() / scale;

			std::vector<chili::Color> pColor;
			pColor.resize(size_t(new_width * new_height));

			for (int y = 0; y < sprite.GetHeight(); y += scale)
			{
				for (int x = 0; x < sprite.GetWidth(); x += scale)
				{

					unsigned int r, g, b;
					r = g = b = 0u;

					// blending a ScaleSize x ScaleSize rect
					for (int y0 = y; y0 < y + scale; y0++)
					{
						for (int x0 = x; x0 < x + scale; x0++)
						{
							const chili::Color c = sprite.GetPixel(int(x0), int(y0));
							r += c.GetR();
							g += c.GetG();
							b += c.GetB();
						}
					}
					auto Area = scale * scale;
					r /= Area;
					g /= Area;
					b /= Area;

					const chili::Color new_color = chili::Color(r, g, b);

					pColor[size_t((y / scale) * new_width + x / scale)] = new_color;
				}
			}
			return Sprite(Size<short>(new_width, new_height), pColor);
		}
		void UpSize()
		{
			int new_width = downBuffer.GetWidth() * ScaleSize;
			int new_height = downBuffer.GetHeight() * ScaleSize;

			std::vector<chili::Color> pColor;
			pColor.resize(size_t(new_width * new_height));

			for (int y = 0; y < downBuffer.GetHeight(); y += 1)
			{
				for (int x = 0; x < downBuffer.GetWidth(); x += 1)
				{
					// filling a ScaleSize x ScaleSize rect
					for (int y0 = y * ScaleSize; y0 < int(y * ScaleSize + ScaleSize); y0++)
					{
						for (int x0 = x * ScaleSize; x0 < int(x * ScaleSize + ScaleSize); x0++)
						{
							const chili::Color c = downBuffer.GetPixel(x, y);
							pColor[size_t((y0)*new_width + x0)] = c;
						}
					}
				}
			}
			bloomed_sprite = Sprite(Size<short>(new_width, new_height), pColor);
		}
		void HorizontalPass()
		{
			chili::Color* pColor = downBuffer.Data();
			for (int y = 0; y < downBuffer.GetHeight(); y += 1)
			{
				int index_height = y * downBuffer.GetWidth();

				for (int x = 0; x < downBuffer.GetWidth() - diameter / 2; x += 1)
				{
					for (int i = 0; i < diameter; i++)
					{
						chili::Color& c = pColor[index_height + x + i];
						c = chili::Color(minimum(unsigned int(c.GetR() * (kernel[i] + 1) / sumKernel), 255u),
							minimum(unsigned int(c.GetG() * (kernel[i] + 1) / sumKernel), 255u),
							minimum(unsigned int(c.GetB() * (kernel[i] + 1) / sumKernel), 255u));
					}
				}
			}
		}
		void VerticalPass()
		{
			chili::Color* pColor = downBuffer.Data();
			for (int y = 0; y < downBuffer.GetHeight() - diameter / 2; y += 1)
			{
				int index_height = y * downBuffer.GetWidth();

				for (int x = 0; x < downBuffer.GetWidth(); x += 1)
				{
					for (int i = 0; i < diameter; i++)
					{
						chili::Color& c = pColor[index_height + x + i * downBuffer.GetWidth()];
						c = chili::Color(minimum(unsigned int(c.GetR() * (kernel[i] + 1) / sumKernel), 255u),
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
		unsigned int ScaleSize = 20u;
		static constexpr int diameter = 8;
		unsigned int kernel[diameter];
		unsigned int sumKernel = 0;
		unsigned int OverDriveFactor = 2;
		std::function<int(int, int)> minimum = [](int x1, int x2) {
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
}