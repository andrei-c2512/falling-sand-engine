#pragma once

#include "chili/Graphics.hpp"
#include "chili/MainWindow.hpp"

#include "engine2d/core/GraphicsAPI.hpp"
#include "engine2d/math/Rect.hpp"
#include "engine2d/math/Vec2d.hpp"
#include "engine2d/render/Sprite.hpp"
#include "engine2d/render/BlurProcessor.hpp"
#include "engine2d/render/Font.hpp"

namespace e2d {
	class Renderer : public chili::Graphics {
		public:	
			Renderer(chili::MainWindow& wnd)
				:chili::Graphics(wnd)
			{}
			// I would use DrawText but there is a windows macro named lke this
			void DrawString(int x, int y, const char* text, Font font) {
				for (int i = 0 ; i < strlen(text) ; i++)
				{
					char c = text[i];
					if (c >= Font::FirstLetter && c <= Font::LastLetter)
					{
						const size_t index = c - Font::FirstLetter;
						RectI Letter = font.GetLetterRect(index);

						DrawSprite(x, y, font.GetSpriteSheet(), Letter, GetScreenRect());
						x += font.GetLetterDim().width;
					}
				}
			}
			
			void DrawSprite(int x, int y, const Sprite& s, RectI SpritePortion, const RectI& clip)
			{
				assert(WithinScreen(SpritePortion));

				if (clip.left > x)
				{
					SpritePortion.left += std::abs(clip.left - x);
					SpritePortion.width -= std::abs(clip.left - x);
					x = clip.left;
				}
				if (clip.bottom > y)
				{
					SpritePortion.bottom += clip.bottom - y;
					SpritePortion.height -= clip.bottom - y;
					y = clip.bottom;
				}

				if (clip.right() < x + SpritePortion.width)
				{
					SpritePortion.width -= std::abs(x + SpritePortion.width - clip.right());
				}
				if (clip.top() < y + SpritePortion.height)
				{
					SpritePortion.height -= std::abs(y + SpritePortion.height - clip.top());
				}

				for (int sy = SpritePortion.bottom; sy < SpritePortion.top(); sy++)
				{
					for (int sx = SpritePortion.left; sx < SpritePortion.right(); sx++)
					{
						chili::Color c = s.GetPixel(sx, sy);
						//effect(std::move(c), *this, pos.x + sx - SpritePortion.left, pos.y + sy - SpritePortion.bottom);
						chili::Graphics::PutPixel(x + sx - SpritePortion.left, y + sy - SpritePortion.bottom, c);
					}
				}
			}
			static RectI GetScreenRect() {
				return RectI(Vec2I(0.0f, 0.0f), Vec2I(Graphics::ScreenWidth, Graphics::ScreenHeight));
			}
			inline void DrawRect(int x, int y, int width, int height, chili::Color c)
			{
				for (int sy = y; sy < y + height; sy++)
				{
					for (int sx = x; sx < x + width; sx++)
						chili::Graphics::PutPixel(sx, sy, c);
				}
			}
			inline void DrawRectA(int x, int y, int width, int height, chili::Color c) {
				for (int sy = y; sy < y + height; sy++)
				{
					for (int sx = x; sx < x + width; sx++)
						chili::Graphics::PutPixel(sx, sy, c);
				}
			}

			void DrawElementRect(RectI rect, chili::Color c, const RectI& clip = GetScreenRect())
			{
				for (int y = rect.bottom; y < rect.top(); y++)
				{
					for (int x = rect.left; x < rect.right(); x++)
						chili::Graphics::PutPixel(x, y, c);
				}
			}
			template<typename T, typename D>
			void DrawRectTransparent(Rect_<T, D> rect, chili::Color c, const Sprite& sprite, int transparency, const RectI& clip = GetScreenRect())
			{
				RectI new_rect = RectI(rect.width, rect.height, Vec2I(rect.left, rect.top));

				if (int(rect.bottom) <= clip.bottom)
				{
					int dif = clip.bottom - rect.bottom;
					new_rect.bottom += dif;
					new_rect.height -= dif;
				}
				if (int(rect.left) <= clip.left)
				{
					int dif = clip.left - rect.left;
					new_rect.left += dif;
					new_rect.width -= dif;
				}
				if (int(rect.top()) > clip.top())
				{
					new_rect.height += (clip.top() - rect.top() - 1);
				}
				if (int(rect.right()) > clip.right())
				{
					new_rect.width += (clip.right() - rect.right() - 1);
				}
				float tFactor = float(transparency / 100.0f);

				for (int y = new_rect.top; y < new_rect.bottom(); y++)
				{
					for (int x = new_rect.left; x < new_rect.right(); x++)
					{
						chili::Color sColor = sprite.GetPixel(x, y);
						int r = (c.GetR() + sColor.GetR()) * tFactor, b = (c.GetB() + sColor.GetB()) * tFactor,
							g = (c.GetG() + sColor.GetG()) * tFactor;
						PutPixel(int(x), int(y), r, g, b);
					}
				}
			}
			template<typename T, typename D>
			void DrawRectTransparent(Rect_<T, D> rect, chili::Color c, int transparency, const RectI& clip = GetScreenRect())
			{
				RectI new_rect = RectI(rect.width, rect.height, Vec2I(rect.left, rect.bottom));

				if (int(rect.bottom) <= clip.bottom)
				{
					int dif = clip.bottom - rect.bottom;
					new_rect.bottom += dif;
					new_rect.height -= dif;
				}
				if (int(rect.left) <= clip.left)
				{
					int dif = clip.left - rect.left;
					new_rect.left += dif;
					new_rect.width -= dif;
				}
				if (int(rect.top()) > clip.top())
				{
					new_rect.height += (clip.top() - rect.top() - 1);
				}
				if (int(rect.right()) > clip.right())
				{
					new_rect.width += (clip.right() - rect.right() - 1);
				}

				float tFactor = float(transparency / 100.0f);
				for (int y = new_rect.bottom; y < new_rect.top(); y++)
				{
					for (int x = new_rect.left; x < new_rect.right(); x++)
					{
						chili::Color sColor = Color(pSysBuffer[y * ScreenWidth + x].GetR() * (1.0f - tFactor),
							pSysBuffer[y * ScreenWidth + x].GetG() * (1.0f - tFactor),
							pSysBuffer[y * ScreenWidth + x].GetB() * (1.0f - tFactor));

						int r = (c.GetR() + sColor.GetR()) * tFactor, b = (c.GetB() + sColor.GetB()) * tFactor,
							g = (c.GetG() + sColor.GetG()) * tFactor;
						PutPixel(int(x), int(y), r, g, b);
					}
				}
			}
			template<typename T, typename D>
			static bool WithinScreen(const Rect_<T, D>& rect) {
				return (rect.left >= 0 && rect.left + rect.width <= Graphics::ScreenWidth &&
					rect.bottom >= 0 && rect.bottom + rect.height <= Graphics::ScreenHeight);

			}
			void DrawRectBorder(int x, int y, int width, int height, chili::Color c, RectI clip = GetScreenRect())
			{
				RectI new_rect = RectI(width, height, Vec2I(x, y));

				if (new_rect.bottom == y)
					for (int x = new_rect.left; x < new_rect.right(); x++)
					{
						chili::Graphics::PutPixel(x, new_rect.bottom, c);
					}

				bool left_border = new_rect.left == x;
				bool right_border = new_rect.right() == x + width;

				for (int y = new_rect.bottom + 1; y < new_rect.top() - 1; y++)
				{

					if (left_border);
						chili::Graphics::PutPixel(new_rect.left, y, c);

					if (right_border)
						chili::Graphics::PutPixel(new_rect.right() - 1, y, c);
				}

				if (new_rect.top() == y + height) {
					for (int x = new_rect.left; x < new_rect.right(); x++)
					{
						chili::Graphics::PutPixel(x, new_rect.bottom - 1, c);
					}
				}
			}
			static bool WithinScreen(const Vec2I& pos)
			{
				return pos.x >= 0 && pos.x <= ScreenWidth && pos.y >= 0 && pos.y <= ScreenHeight;
			}
			template<typename T, typename D>
			void DrawAngledSprite(int x, int y, const Sprite& s, const Rect_<T, D>& SpritePortion, const Vec2I& pos)
			{
				const short StartX = SpritePortion.left - SpritePortion.width / 2;
				const short StartY = SpritePortion.bottom - SpritePortion.height / 2;

				const short StopY = SpritePortion.bottom + SpritePortion.height / 2;
				const short StopX = SpritePortion.left + SpritePortion.width / 2;

				RectI SRect = { SpritePortion.width , SpritePortion.height , Vec2I{x , y } };
				Vec2I SCenter = SRect.GetCenter();

				float hyphotenuse = SCenter.GetLenght(pos);
				float sin = SCenter.GetSin(pos, hyphotenuse);
				float cos = SCenter.GetCos(pos, hyphotenuse);

				short hWidth = SpritePortion.width / 2;
				short hHeight = SpritePortion.height / 2;
				for (short sy = StartY; sy < StopY; sy++)
				{
					for (short sx = StartX; sx < StopX; sx++)
					{
						chili::Color c = s.GetPixel(sx + hWidth, sy + hHeight);

						Vec2I new_pos = Vec2I(SCenter.x + sx * cos - sy * sin,
							SCenter.y + sx * sin + sy * cos);

						if (WithinScreen(new_pos))
						{
							PutPixel(std::move(new_pos.x), std::move(new_pos.y), c);
						}
					}
				}
			}
			template<typename T, typename D, typename P>
			void DrawAngledSprite(Vec2_<P> pos1, const Sprite& s, const Rect_<T, D>& SpritePortion, const Vec2I& pos2)
			{
				//pos1 is the position from where its drawn and pos2 is the point that its facing
				const short StartX = SpritePortion.left - SpritePortion.width / 2;
				const short StartY = SpritePortion.bottom - SpritePortion.height / 2;

				const short StopY = SpritePortion.bottom + SpritePortion.height / 2;
				const short StopX = SpritePortion.left + SpritePortion.width / 2;

				RectI SRect = { SpritePortion.width , SpritePortion.height , Vec2I(pos1.x , pos1.y) };
				Vec2I SCenter = SRect.GetCenter();

				float hyphotenuse = SCenter.GetLenght(pos2);
				float sin = SCenter.GetSin(pos2, hyphotenuse);
				float cos = SCenter.GetCos(pos2, hyphotenuse);

				short hWidth = SpritePortion.width / 2;
				short hHeight = SpritePortion.height / 2;
				for (short sy = StartX; sy < StopY; sy++)
				{
					for (short sx = StartY; sx < StopX; sx++)
					{
						chili::Color c = s.GetPixel(sx + hWidth, sy + hHeight);

						Vec2I new_pos = Vec2I(SCenter.x + sx * cos - sy * sin,
							SCenter.y + sx * sin + sy * cos);

						if (WithinScreen(new_pos))
						{
							PutPixel(std::move(new_pos.x), std::move(new_pos.y), c);
						}
					}
				}
			}
			void BlendPixel(int x, int y, chili::Color c)
			{
				// improve this pls
				int alpha = c.GetA();
				float tFactor = float(alpha / 100.0f);
				int index = y * chili::Graphics::ScreenWidth + x;
				chili::Color sColor = chili::Color(pSysBuffer[index].GetR() * (1.0f - tFactor),
					pSysBuffer[index].GetG() * (1.0f - tFactor),
					pSysBuffer[index].GetB() * (1.0f - tFactor));

				int r = (c.GetR() + sColor.GetR()) * tFactor, b = (c.GetB() + sColor.GetB()) * tFactor,
					g = (c.GetG() + sColor.GetG()) * tFactor;

				PutPixel(index, chili::Color(r, g, b));
			}
			void PutPixel(unsigned int index, chili::Color c)
			{
				if (index < Graphics::ScreenWidth * chili::Graphics::ScreenHeight)
					pSysBuffer[index] = c;
			}
			//template<typename T, typename D>
			//void DrawAngledSprite(int x0, int y0, Sprite& s, Rect_<T, D>& SpritePortion, Vec2I& pos)
			//{
			//	float yDist = pos.y - y0;
			//	float xDist = pos.x - x0;
			//	float Slope = yDist / xDist; // the tangent
			//	Vec2I origin = { x0 , y0 };
			//
			//	// slope1 * slope2 = -1
			//	// slope1 = -1 / slope2 AKA slope1 = -ctg 
			//	if (std::abs(xDist) > std::abs(yDist))
			//	{
			//		int StartY = y0 - Slope * x0;
			//		float pSlope = -1 / Slope; // perpendicular slope
			//		
			//		float cos = sqrt(1.0f / (Slope * Slope + 1)); // you can get this formula with the fact that tg = sin/cos and
			//													  // sin * sin + cos * cos = 1
			//													  // tg = slope
			//		if (xDist < 0)
			//		{
			//			cos = -cos;
			//		}
			//		float sin = sqrt(1.0f - cos * cos) * (std::abs(yDist) / yDist);
			//
			//		if (yDist < 0)
			//		{
			//			sin = -sin;
			//		}
			//		Vec2I p2 = Vec2I(origin.x + cos * s.GetWidth(), origin.y + sin * s.GetWidth());
			//
			//		if (origin.x > p2.x)
			//		{
			//			std::swap(origin, p2);
			//		}
			//
			//		for (int x = origin.x; x < p2.x; x++)
			//		{
			//			//PutPixel(x, StartY + Slope * x, Colors::Yellow);
			//			DrawLine(Vec2I(x, StartY + Slope * x), s.GetHeight(), pSlope);
			//		}
			//	}
			//	else
			//	{
			//		float pSlope = -1 / Slope;
			//
			//		Slope = xDist / yDist; // switching to cotangent
			//
			//		int StartX = x0 - Slope * y0;
			//	
			//		float cos = sqrt(1.0f / ((1.0f / (Slope * Slope)) + 1)) ;
			//		if (xDist < 0)
			//		{
			//			cos = -cos;
			//		}
			//		float sin = sqrt(1.0f - cos * cos) ;
			//		if (yDist < 0)
			//		{
			//			sin = -sin;
			//		}
			//		Vec2I p2 = Vec2I(origin.x + cos * s.GetWidth(), origin.y + sin * s.GetWidth());
			//
			//		if (origin.y > p2.y)
			//		{
			//			std::swap(p2, origin);
			//		}
			//
			//		for (int y = origin.y; y < p2.y; y++)
			//		{
			//			//PutPixel(StartX + y * Slope, y, Colors::Yellow);
			//			DrawLine(Vec2I(StartX + y * Slope, y), s.GetHeight(), pSlope);
			//		}
			//	}
			//}
			void ChangePixelBloom(int x, int y, chili::Color c)
			{
				int index = y * chili::Graphics::ScreenWidth + x;
				Bloom.PutPixel(index, c);
				PutPixel(index, c);
			}

			void DrawRectIBloom(RectI& rect, chili::Color c)
			{
				for (int y = rect.bottom; y < rect.top(); y++)
				{
					for (int x = rect.left; x < rect.left + rect.width; x++)
						ChangePixelBloom(x, y, c);
				}
			}
			void DrawOpenPoly(std::vector<Vec2I> pos_list)
			{
				for (int i = 0; i < pos_list.size() - 1; i++)
				{
					DrawLine(pos_list[i], pos_list[i + 1], chili::Colors::Yellow);
				}
			}
			void ApplyBloom()
			{
				blur_processor.Go();
				const chili::Color* pColor = Bloom.Data();
				int r, g, b;
				for (size_t ind = 0; ind < chili::Graphics::ScreenHeight * chili::Graphics::ScreenWidth; ind++)
				{

					r = pSysBuffer[ind].GetR() + pColor[ind].GetR();
					if (r > 255)
					{
						r = 255;
					}
					g = pSysBuffer[ind].GetG() + pColor[ind].GetG();
					if (g > 255)
					{
						g = 255;
					}
					b = pSysBuffer[ind].GetB() + pColor[ind].GetB();
					if (r > 255)
					{
						b = 255;
					}

					pSysBuffer[ind] = chili::Color(r, g, b);
				}

			}
			void ResetBloom()
			{
				Bloom.Fill(chili::Colors::Black);
			} 
			template <typename T>
			void DrawLine(Vec2_<T> p1, int length, float slope)
			{
				float cos = sqrt(1.0f / (slope * slope + 1));
				float sin = sqrt(1.0f - cos * cos);

				Vec2I p2 = Vec2I(p1.x + cos * length, p1.y + sin * length);
				DrawLine(p1, p2, chili::Colors::Yellow);
			}
			template <typename T>
			void DrawLine(Vec2_<T> p1, Vec2_<T> p2, chili::Color c)
			{
				float yDist = p2.y - p1.y;
				float xDist = p2.x - p1.x;
				float Slope = 0.0f;

				if (std::abs(xDist) > std::abs(yDist))
				{
					Slope = yDist / xDist; // the tangent
					int StartY = p1.y - Slope * p1.x;

					if (p1.x > p2.x)
					{
						std::swap(p1, p2);
					}
					for (int x = p1.x; x < p2.x; x++)
					{
						chili::Graphics::PutPixel(x, Slope * x + StartY, c);
					}
				}
				else
				{
					Slope = xDist / yDist; // the cotangent

					int StartX = p1.x - Slope * p1.y;

					if (p1.y > p2.y)
					{
						std::swap(p1, p2);
					}

					for (int y = p1.y; y < p2.y; y++)
					{
						chili::Graphics::PutPixel(Slope * y + StartX, y, c);
					}
				}
			}
	private:
		class CoordinateTransformer {
		public:
			template<typename T>
			Vec2_<T> Transform(Vec2_<T> pos)
			{
				pos.x += ScreenWidth / 2;
				pos.y += ScreenHeight / 2;
				return pos;
			}
			template<typename T>
			Vec2_<T> Transform(T x, T y)
			{
				x += ScreenWidth / 2;
				y += ScreenHeight / 2;
				return Vec2_<T>(x, y);
			}
			int TransformX(int x)
			{
				return x + ScreenWidth / 2;
			}
			int TransformY(int y)
			{
				return -y + ScreenHeight / 2;
			}
		};
	protected:
		BlurProcessor blur_processor = { Bloom };
		CoordinateTransformer ct;
		Sprite Bloom = Sprite(ScreenWidth, ScreenHeight);
	public:
		static constexpr int WorldWidth = chili::Graphics::ScreenWidth * 2;
		static constexpr int WorldHeight = chili::Graphics::ScreenHeight * 2;

		//adding half of the screen dimensions because the Origin is in the center of the screen
		static constexpr RectI WorldArea = RectI(WorldWidth, WorldHeight,
			Vec2I(-WorldWidth / 2, -WorldHeight / 2));
	};
}
