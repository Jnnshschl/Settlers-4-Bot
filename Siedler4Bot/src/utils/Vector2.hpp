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

	inline bool operator==(const Vector2& rhs) { return X == rhs.X && Y == rhs.Y; }
	inline bool operator!=(const Vector2& rhs) { return !(*this == rhs); }

	inline Vector2 operator+(const Vector2& other) noexcept { return Vector2{ static_cast<unsigned short>(X + other.X), static_cast<unsigned short>(Y + other.Y) }; }
	inline Vector2 operator-(const Vector2& other) noexcept { return Vector2{ static_cast<unsigned short>(X - other.X), static_cast<unsigned short>(Y - other.Y) }; }
	inline Vector2 operator*(const Vector2& other) noexcept { return Vector2{ static_cast<unsigned short>(X * other.X), static_cast<unsigned short>(Y * other.Y) }; }
	inline Vector2 operator/(const Vector2& other) noexcept { return Vector2{ static_cast<unsigned short>(X / other.X), static_cast<unsigned short>(Y / other.Y) }; }

	inline Vector2 operator+(int other) const noexcept { return Vector2{ static_cast<unsigned short>(X + other), static_cast<unsigned short>(Y + other) }; }
	inline Vector2 operator-(int other) const noexcept { return Vector2{ static_cast<unsigned short>(X - other), static_cast<unsigned short>(Y - other) }; }
	inline Vector2 operator*(int other) const noexcept { return Vector2{ static_cast<unsigned short>(X * other), static_cast<unsigned short>(Y * other) }; }
	inline Vector2 operator/(int other) const noexcept { return Vector2{ static_cast<unsigned short>(X / other), static_cast<unsigned short>(Y / other) }; }

	inline Vector2& operator+=(const Vector2& other) noexcept
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	inline Vector2& operator-=(const Vector2& other) noexcept
	{
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	inline Vector2& operator*=(const Vector2& other) noexcept
	{
		X *= other.X;
		Y *= other.Y;
		return *this;
	}

	inline Vector2& operator/=(const Vector2& other) noexcept
	{
		X /= other.X;
		Y /= other.Y;
		return *this;
	}
	
	inline Vector2& operator+=(int other) noexcept
	{
		X += other;
		Y += other;
		return *this;
	}

	inline Vector2& operator-=(int other) noexcept
	{
		X -= other;
		Y -= other;
		return *this;
	}

	inline Vector2& operator*=(int other) noexcept
	{
		X *= other;
		Y *= other;
		return *this;
	}

	inline Vector2& operator/=(int other) noexcept
	{
		X /= other;
		Y /= other;
		return *this;
	}

	inline unsigned short DistanceTo(const Vector2& other) const noexcept
	{
		return CalcDistance(X, Y, other.X, other.Y);
	}

	inline unsigned long Packed() const noexcept
	{
		return PackShorts(X, Y);
	}
};
