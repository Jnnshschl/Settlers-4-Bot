#pragma once

#include <vector>

#include "../../utils/Vector2.hpp"

struct ResourceSpot
{
	std::vector<Vector2> Spots{};

	explicit ResourceSpot(Vector2 firstResourcePos) noexcept
	{
		Spots.push_back(firstResourcePos);
	}

	constexpr auto Amount() const noexcept { return Spots.size(); }

	bool IsNearPool(const Vector2& pos, unsigned short threshold = 16) const noexcept
	{
		for (const auto& spot : Spots)
		{
			if (pos.DistanceTo(spot) < threshold)
			{
				return true;
			}
		}

		return false;
	}

	Vector2 CenterPosition() const noexcept
	{
		unsigned short xSum = 0;
		unsigned short ySum = 0;

		for (const auto& spot : Spots)
		{
			xSum += spot.X;
			ySum += spot.Y;
		}

		const unsigned short count = static_cast<unsigned short>(Spots.size());
		return { static_cast<unsigned short>(xSum / count), static_cast<unsigned short>(ySum / count) };
	}
};
