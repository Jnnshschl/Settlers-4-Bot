#pragma once

#include "BuildAction.hpp"
#include "../../../../../../sdk/S4Api.hpp"

class StaticBuildAction : public BuildAction
{
	S4Building Building;
	Vector2 Position;

public:
	StaticBuildAction(S4Api* s4, S4Building building, const Vector2& position)
		: BuildAction(s4),
		Building(building),
		Position(position)
	{}

	virtual bool Execute() noexcept override
	{
		S4->BuildAtClosestSpot(Building, Position);
		Done = true;
		return true;
	}
};
