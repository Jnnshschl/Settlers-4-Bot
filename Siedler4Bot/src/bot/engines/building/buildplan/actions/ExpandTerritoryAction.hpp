#pragma once

#include "SimpleAction.hpp"
#include "../../../../../sdk/S4Api.hpp"

class ExpandTerritoryAction : public SimpleAction
{
	S4Building Tower;
	Vector2 Position;
	Vector2 TowerPosition;
	Vector2 LastTowerPosition;

public:
	ExpandTerritoryAction(S4Api* s4, const Vector2& position, S4Building tower = S4Building::SmallTower)
		: SimpleAction(s4),
		Tower(tower),
		Position(position),
		TowerPosition(0, 0),
		LastTowerPosition(USHRT_MAX, USHRT_MAX)
	{}

	virtual bool Execute() noexcept override
	{
		// TODO: use pathfinding for this, as there may occur deadlocks using this technique
		if (S4->FindClosestBuildingSpot(Position, Tower, TowerPosition, 4096))
		{
			if (TowerPosition.DistanceTo(LastTowerPosition) > 20)
			{
				LastTowerPosition = TowerPosition;
				S4->SendNetEvent(S4EventId::BuildingBuild, static_cast<unsigned long>(Tower), TowerPosition.Packed());
			}

			const auto owner = S4->LandscapeOwner(Position);

			// anyone claimed the territory
			if (owner > 0)
			{
				Done = true;
				CallResultHandlers(SimpleActionResult::OK, Position);
			}
			else
			{
				CallResultHandlers(SimpleActionResult::IN_PROGRESS, TowerPosition);
			}

			return owner == S4->LocalPlayerId(); // was it me?
		}

		Done = true;
		CallResultHandlers(SimpleActionResult::ERROR_NO_SPACE, Position);
		return false;
	}
};
