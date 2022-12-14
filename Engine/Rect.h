#pragma once
#include "Vec2D.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Dimensions.h"
template <typename T , typename D>
class Rect_ {
public:
	Rect_() = default;
	Rect_(T left0, T top0, T right0, T bottom0)
		:left(left0),
		top(top0),
		width(D(std::abs(right0 - left0))),
		height(D(std::abs(top0 - bottom0)))
	{
	}

	Rect_(Vec2_<T>& TopLeft0, Vec2_<T>& BottomRight0)
		:left(TopLeft0.x),
		top(TopLeft0.y),
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
			top = BottomRight0.y;
		}
		else
			top = TopLeft0.y;

	}

	Rect_(D width0, D height0, Vec2_<T>& pos)
		:left(pos.x), top(pos.y),
		width(width0), height(height0)
	{
	}
	Rect_(Dimensions<D> dim, Vec2_<T>& pos)
		:left(pos.x), top(pos.y),
		width(dim.width), height(dim.height)
	{
	}
	
    bool Collision(Rect_& rect) const
	{
		return (left + width >= rect.left && left < rect.left + rect.width &&
			top + height >= rect.top && top < rect.top + rect.height);
	}

	bool Hovered(Mouse& mouse)
	{
		const T x = mouse.GetPosX(), y = mouse.GetPosY();
		return (x <= left + width && x >= left &&
			y <= top + height && y >= top);
	}

	bool operator==(Rect_& rect) const
	{
		return (rect.left == left && rect.top == top && rect.width == width && rect.height == height);
	}

	bool PointInRect(Vec2_<T>& point) const {
		return (point.x <= left + width && point.x >= left &&
			point.y <= top + height && point.y >= top);
	}

	Rect_ operator* (T number) {
		return Rect_(width * number, height * number,
			Vec2_<T>(T(left * number), T(top * number)));
	}

	Rect_ operator/ (T number) {
		return Rect_(width / number, height / number,
			Vec2_<T>(T(left / number), T(top / number)));
	}

	void SwapPositions(Rect_& rect)
	{
		Vec2I pos = { left , top };
		left = rect.left;
		top = rect.top;

		rect.left = pos.x;
		rect.top = pos.y;
	}

	T right() const
	{
		return width + left;
	}
	T bottom() const
	{
		return height + top;
	}
	Vec2_<T> pos()const {
		return Vec2_<T>(left, top);
	}
	Dimensions<D> GetDimensions() const {
		return Dimensions<D>( D(width)  , D(height) );
	}
	Vec2_<T> GetCenter() 
	{
		return Vec2_<T>(left + width / 2, top + height / 2);
	}
	Vec2_<T> GetPos()
	{
		return Vec2<T>_(left, top);
	}

	void SetPos(Vec2_<T> newpos)
	{
		left = newpos.left;
		top = newpos.top
	}
public:
	T left = T(0);
	T top = T(0);
	 
	D width = D(0);
	D height = D(0);
};

typedef Rect_<float ,short> Rect;
typedef Rect_<int   ,short> RectI;
