#pragma once
#include "engine2d/math/Vec2D.hpp"
#include "engine2d/math/Size.hpp"
#include "chili/Mouse.hpp"

namespace e2d {
	template <typename T, typename D>
	class Rect_ {
	public:
		constexpr Rect_() = default;
		constexpr Rect_(T left0, T top0, T right0, T bottom0)
			:left(left0),
			bottom(top0),
			width(D(std::abs(right0 - left0))),
			height(D(std::abs(top0 - bottom0)))
		{
		}

		constexpr Rect_(const Vec2_<T>& TopLeft0, const Vec2_<T>& BottomRight0)
			:left(TopLeft0.x),
			bottom(TopLeft0.y),
			width(T(std::abs(TopLeft0.x - BottomRight0.x))),
			height(T(std::abs(TopLeft0.y - BottomRight0.y)))
		{
			if (TopLeft0.x > BottomRight0.x)
			{
				left = BottomRight0.x;
			}
			else
				left = TopLeft0.x;

			if (TopLeft0.y > BottomRight0.y)
			{
				bottom = BottomRight0.y;
			}
			else
				bottom = TopLeft0.y;

		}

		constexpr Rect_(D width0, D height0, const Vec2_<T>& pos)
			:left(pos.x), bottom(pos.y),
			width(width0), height(height0)
		{
		}
		constexpr Rect_(Size<D> dim, const Vec2_<T>& pos)
			: left(pos.x), bottom(pos.y),
			width(dim.width), height(dim.height)
		{
		}

		bool Collision(const Rect_& rect) const
		{
			return (left + width >= rect.left && left < rect.left + rect.width &&
				bottom + height >= rect.bottom && bottom < rect.bottom + rect.height);
		}

		bool Hovered(const chili::Mouse& mouse)
		{
			const T x = mouse.GetPosX(), y = mouse.GetPosY();
			return (x <= left + width && x >= left &&
				y <= bottom + height && y >= bottom);
		}

		bool operator==(const Rect_& rect) const
		{
			return (rect.left == left && rect.bottom == bottom && rect.width == width && rect.height == height);
		}

		bool PointInRect(const Vec2_<T>& point) const {
			return (point.x <= left + width && point.x >= left &&
				point.y >= bottom - height && point.y <= bottom);
		}

		Rect_ operator* (T number) {
			return Rect_(width * number, height * number,
				Vec2_<T>(T(left * number), T(bottom * number)));
		}

		Rect_ operator/ (T number) {
			return Rect_(width / number, height / number,
				Vec2_<T>(T(left / number), T(bottom / number)));
		}

		void SwapPositions(const Rect_& rect)
		{
			Vec2I pos = { left , bottom };
			left = rect.left;
			bottom = rect.bottom;

			rect.left = pos.x;
			rect.bottom = pos.y;
		}

		T right() const
		{
			return width + left;
		}
		// we name it like this because we have a coordinate transformer that inverses the y coordinates
		T top() const
		{
			return height + bottom;
		}
		Vec2_<T> pos()const {
			return Vec2_<T>(left, bottom);
		}
		Size<D> GetSize() const {
			return Size<D>(D(width), D(height));
		}
		Vec2_<T> GetCenter()
		{
			return Vec2_<T>(left + width / 2, bottom + height / 2);
		}
		Vec2_<T> GetPos() const
		{
			return Vec2_<T>(left, bottom);
		}

		void SetPos(Vec2_<T> newpos)
		{
			left = newpos.x;
			bottom = newpos.y;
		}

		Rect_<T, D> ClipTo(const Rect_<T, D>& clip) const {
			auto rect = *this;
			if (rect.bottom <= clip.bottom)
			{
				int dif = clip.bottom - rect.bottom;
				rect.bottom += dif;
				rect.height -= dif;
			}
			if (rect.left <= clip.left)
			{
				int dif = clip.left - rect.left;
				rect.left += dif;
				rect.width -= dif;
			}
			if (rect.bottom + rect.height > clip.top())
			{
				rect.height += (clip.top() - (rect.bottom + rect.height) - 1);
			}
			if (rect.left + rect.width > clip.right())
			{
				rect.width += (clip.right() - (rect.left + rect.width) - 1);
			}
			return rect;
		}
	public:
		T left = T(0);
		T bottom = T(0);

		D width = D(0);
		D height = D(0);
	};

	typedef Rect_<float, short> Rect;
	typedef Rect_<int, short> RectI;
}
