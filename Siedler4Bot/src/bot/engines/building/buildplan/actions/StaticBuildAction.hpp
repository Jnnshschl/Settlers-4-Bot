#pragma once

#include "SimpleAction.hpp"
#include "../../../../../sdk/S4Api.hpp"

class StaticBuildAction : public SimpleAction
{
	int Count;
	int MaxDistance;
	int MinDistance;
	S4Building Building;
	Vector2 Position;

public:
	StaticBuildAction(S4Api* s4, S4Building building, const Vector2& position, int count = 1, int maxDistance = 256, int minDistance = 0)
		: SimpleAction(s4),
		Count(count),
		MaxDistance(maxDistance),
		MinDistance(minDistance),
		Building(building),
		Position(position)
	{}

	virtual bool Execute() noexcept override
	{
		Done == --Count <= 0;

		if (S4->BuildAtClosestSpot(Building, Position, MaxDistance, MinDistance))
		{
			CallResultHandlers(SimpleActionResult::OK, Position);
			return true;
		}

		Done = true;
		CallResultHandlers(SimpleActionResult::ERROR_NO_SPACE, Position);
		return false;
	}
};
