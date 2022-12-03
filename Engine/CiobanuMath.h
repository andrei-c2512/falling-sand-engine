#define PI 3.14159265f
#include <cmath>

template <typename T>
float sq(T x)
{
	return x * x;
}

template <typename T>
float gaussian(T x, T o)
{
	return (1.0f / sqrt(2.0f * PI * sq(o))) * exp(-sq(x) / (2.0f * sq(o)));
}

