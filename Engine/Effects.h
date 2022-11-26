#pragma once
#include "Colors.h"
#include "Graphics.h"

namespace Effects{
	class Chroma {
	public:
		Chroma(Color chroma0)
			:chroma(chroma0)
		{

		}
		void operator()(Color color, Graphics& gfx, int x, int y)
		{
			if (color != chroma)
				gfx.PutPixel(x, y, color);
		}
	private:
		Color chroma;
	};

	class Copy {
	public:
		void operator()(Color color, Graphics& gfx, int x, int y)
		{
			gfx.PutPixel(x, y, color);
		}
	};

	class OneColor {
	public:
		OneColor(Color color0, Color chroma0)
			:color(color0), chroma(chroma0)
		{

		}
		void operator()(Color color0, Graphics& gfx, int x, int y) {
			if (color0 != chroma)
			{
				gfx.PutPixel(x, y, color);
			}
		}
	private:
		Color color;
		Color chroma;
	};
}