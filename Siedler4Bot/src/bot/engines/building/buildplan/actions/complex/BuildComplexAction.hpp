#pragma once

#include "../SimpleAction.hpp"
#include "../StaticBuildAction.hpp"
#include "../../plans/BuildEntry.hpp"

class BuildComplexAction : public SimpleAction
{
	std::deque<IAction*>* Actions;
	std::vector<BuildEntry> Buildings;

public:
	BuildComplexAction(S4Api* s4, std::deque<IAction*>* actions, std::vector<BuildEntry> buildings)
		: SimpleAction(s4),
		Actions(actions),
		Buildings(buildings)
	{}

	virtual bool GetPosition(Vector2& pos) = 0;

	virtual bool Execute() noexcept override
	{
		Done = true;
		Vector2 pos;

		if (GetPosition(pos))
		{
			for (const auto& b : Buildings)
			{
				Actions->push_back(new StaticBuildAction(S4, b.Building, pos, b.Count));
			}

			CallResultHandlers(SimpleActionResult::OK, pos);
			return true;
		}

		CallResultHandlers(SimpleActionResult::ERROR_NO_RESOURCE, Vector2());
		return false;
	}
};
