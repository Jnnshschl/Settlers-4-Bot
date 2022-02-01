#pragma once

#include "Utils.hpp"
#include "MathUtils.hpp"

struct Vector2
{
	unsigned short X;
	unsigned short Y;

	Vector2()
		: X(0), Y(0)
	{}

	Vector2(unsigned short x, unsigned short y)
		: X(x), Y(y)
	{}

	inline unsigned short DistanceTo(const Vector2& other) const noexcept
	{
		return CalcDistance(X, Y, other.X, other.Y);
	}

	inline unsigned long Packed() const noexcept
	{
		return PackShorts(X, Y);
	}
};
