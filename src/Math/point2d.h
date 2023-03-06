#pragma once
#include "Math/vector2d.h"

template <typename T>
struct point2d
{
	point2d()
	{
		x = 0;
		y = 0;
	}

	template <typename U, typename V>
	point2d(U v1, V v2)
	{
		x = (T)v1;
		y = (T)v2;
	}

	template <typename U>
	point2d(const point2d<U>& p)
	{
		x = (T)p.x;
		y = (T)p.y;
	}

	bool operator==(const point2d<T>& rhs) const
	{
		return (x == rhs.x && y == rhs.y);
	}

	bool operator!=(const point2d<T>& rhs) const
	{
		return !(*this == rhs);
	}

	point2d<T> operator=(const point2d<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	point2d<T> operator+(const vector2d<T>& v) const
	{
		return point2d<T>(x + v.x, y + v.y);
	}
	vector2d<T> operator-(const point2d<T>& p) const
	{
		return vector2d<T>(x - p.x, y - p.y);
	}
	point2d<T> operator-(const vector2d<T>& p) const
	{
		return point2d<T>(x - p.x, y - p.y);
	}

	T x;
	T y;
};

template <typename T>
point2d<T> componentMultiply(const point2d<T>& lhs, const vector2d<T>& rhs)
{
	return point2d<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

typedef point2d<int> pt2di;
typedef point2d<float> pt2df;
typedef point2d<double> pt2dd;
