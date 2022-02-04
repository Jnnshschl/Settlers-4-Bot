#pragma once

#include "BuildComplexAction.hpp"

class BuildComplexAtLandscapeAction : public BuildComplexAction
{
	S4GroundType Start;
	S4GroundType End;
	bool IncludeOwn;
	bool IncludeUnclaimed;
	bool IncludeEnemy;

public:
	BuildComplexAtLandscapeAction(S4Api* s4, std::deque<IAction*>* actions, std::vector<BuildEntry> buildings, S4GroundType start, S4GroundType end, bool includeOwn = true, bool includeUnclaimed = false, bool includeEnemy = false)
		: BuildComplexAction(s4, actions, buildings),
		Start(start),
		End(end),
		IncludeOwn(includeOwn),
		IncludeUnclaimed(includeUnclaimed),
		IncludeEnemy(includeEnemy)
	{}

	virtual bool GetPosition(Vector2& pos) noexcept override
	{
		return S4->FindClosestTerrain(S4->CalculateSettlementCenter(), Start, End, pos, IncludeOwn, IncludeUnclaimed, IncludeEnemy);
	}
};
