#pragma once

#include "../../../../../sdk/static/S4Building.hpp"

class BuildEntry
{
public:
	S4Building Building;
	int Count;
	int MinDistance;
	int MaxDistance;

	BuildEntry(S4Building building, int count = 1, int minDistance = 0, int maxDistance = USHRT_MAX) noexcept
		: Building(building),
		Count(count),
		MinDistance(minDistance),
		MaxDistance(maxDistance)
	{};
};