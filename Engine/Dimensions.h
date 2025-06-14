#pragma once

template<typename T>
struct Dimensions {
	constexpr Dimensions() = default;
	constexpr Dimensions(T width0, T height0)
		:width(width0) , height(height0)
	{
	}
	T GetArea() const {
		return width * height;
	}
	T width;
	T height;
};