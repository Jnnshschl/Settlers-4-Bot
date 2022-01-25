#pragma once

#include "Utils.hpp"
#include "MathUtils.hpp"

struct S4Position
{
	unsigned short X;
	unsigned short Y;

	S4Position(unsigned short x, unsigned short y)
		: X(x), Y(y)
	{}

	inline unsigned short DistanceTo(const S4Position& other) const noexcept
	{
		return CalcDistance(X, Y, other.X, other.Y);
	}

	inline unsigned long Packed() const noexcept
	{
		return PackShorts(X, Y);
	}
};
