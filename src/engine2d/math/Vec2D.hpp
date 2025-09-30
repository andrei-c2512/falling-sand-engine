#pragma once
#include <cmath>

namespace e2d {
	template<typename T>
	class Vec2_ {
	public:
		constexpr Vec2_() = default;
		constexpr Vec2_(T x0, T y0)
			:
			x(x0),
			y(y0)
		{
		}
		Vec2_ operator+(const Vec2_<T>& dumb) const {
			return Vec2_(x + dumb.x, y + dumb.y);
		}
		Vec2_ operator-(const Vec2_<T>& dumb) const {
			return Vec2_(x - dumb.x, y - dumb.y);
		}
		Vec2_& operator+=(const Vec2_<T>& dumb) {
			return *this = *this + dumb;
		}
		Vec2_ operator*(T val) const {
			return Vec2_(x * val, y * val);
		}
		Vec2_& operator*=(T val) {
			return *this = *this * val;
		}
		float GetLenght(const Vec2_<T>& point1)const {
			const T x0 = x - point1.x;
			const T y0 = y - point1.y;
			return float(sqrt(x0 * x0 + y0 * y0));
		}
		Vec2_& operator+=(T val)
		{
			return *this = Vec2_<T>(x + val, y + val);
		}
		Vec2_& operator/=(T val)
		{
			return *this = Vec2_<T>(x / val, y / val);
		}
		bool operator !=(const Vec2_<T>& vec) const
		{
			return (vec.x != x || vec.y != y);
		}
		bool operator ==(const Vec2_<T>& vec) const
		{
			return (vec.x == x && vec.y == y);
		}
		float GetSlope(const Vec2_<T>& pos)
		{
			return float((float(pos.y - y)) / (float(pos.x - x)));
		}
		float GetSin(const Vec2_<T>& pos) const
		{
			float hyphotenuse = GetLenght(pos);
			return (-(y - pos.y)) / hyphotenuse;     //putting the minus because the screen isn't read in a cartezian method
		}
		float GetCos(const Vec2_<T>& pos) const
		{
			float hyphotenuse = GetLenght(pos);
			return (-(x - pos.x)) / hyphotenuse;
		}
		float GetSin(const Vec2_<T>& pos, float hyphotenuse) const
		{
			return (-(y - pos.y)) / hyphotenuse;
		}
		float GetCos(const Vec2_<T>& pos, float hyphotenuse) const
		{
			return (-(x - pos.x)) / hyphotenuse;
		}
	public:
		T y;
		T x;
	};

	typedef Vec2_<float>  Vec2D;
	typedef Vec2_<int> Vec2I;
}