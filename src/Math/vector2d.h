#pragma once
template <typename T>
struct vector2d
{
	vector2d() { x = 0; y = 0; }
	vector2d(T value1, T value2)
	{
		x = value1;
		y = value2;
	}
	vector2d(const vector2d<T>& v)
	{
		x = v.x;
		y = v.y;
	}

	vector2d<T>& operator=(const vector2d<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	bool operator==(const vector2d<T>& v) const
	{
		return (v.x == x && v.y == y);
	}

	vector2d<T> operator+(const vector2d<T>& v) const
	{
		return vector2d<T>(x + v.x, y + v.y);
	}
	vector2d<T> operator-(const vector2d<T>& v) const
	{
		return vector2d<T>(x - v.x, y - v.y);
	}

	// dot product
	T operator*(const vector2d<T>& rhs) const
	{
		return (x * rhs.x + y * rhs.y);
	}
	// multiplication by a scalar
	vector2d<T> operator*(T s) const
	{
		return vector2d<T>(x * s, y * s);
	}
	// division by a scalar
	vector2d<T> operator/(T s) const
	{
		return vector2d<T>(x / s, y / s);
	}

	// returns the norm of the vector
	T Norm2() const { return (*this) * (*this); }
	T Norm() const { return sqrt(this->Norm2()); }

	bool IsColinear(const vector2d<T>& v) const
	{
		return ((x * v.y - y * v.x) == 0);
	}

	bool IsPositiveMultiple(const vector2d<T>& v) const
	{
		bool isColinear = IsColinear(v);
		T k = 0;
		if (!isColinear) 
			return false;

		if (x != 0)
		{
			k = v.x / x;
		}
		else
		{
			k = v.y / y;
		}

		if (k == 0)
		{
			if (v.x != 0)
			{
				k = x / v.x;
			}
			else
			{
				k = y / v.y;
			}
		}
		return (isColinear && k > 0);
	}
	union 
	{
		T x;
		T w;
	};
	union
	{
		T y;
		T h;
	};
};