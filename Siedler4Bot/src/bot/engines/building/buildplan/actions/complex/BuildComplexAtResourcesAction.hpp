#pragma once

#include "BuildComplexAction.hpp"

class BuildComplexAtResourcesAction : public BuildComplexAction
{
	S4Resource Start;
	S4Resource End;
	bool IncludeOwn;
	bool IncludeUnclaimed;
	bool IncludeEnemy;

public:
	BuildComplexAtResourcesAction(S4Api* s4, std::deque<IAction*>* actions, std::vector<BuildEntry> buildings, S4Resource start, S4Resource end, bool includeOwn = true, bool includeUnclaimed = false, bool includeEnemy = false)
		: BuildComplexAction(s4, actions, buildings),
		Start(start),
		End(end),
		IncludeOwn(includeOwn),
		IncludeUnclaimed(includeUnclaimed),
		IncludeEnemy(includeEnemy)
	{}

	virtual bool GetPosition(Vector2& pos) noexcept override
	{
		auto stonefields = S4->FindResourceSpotsSortByDistance(S4->CalculateSettlementCenter(), Start, End, IncludeOwn, IncludeUnclaimed, IncludeEnemy);

		if (!stonefields.empty())
		{
			pos = stonefields[0].CenterPosition();
			return true;
		}

		return false;
	}
};
