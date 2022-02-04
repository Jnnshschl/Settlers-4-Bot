#pragma once

#include "BuildComplexAction.hpp"

class BuildComplexAtStaticAction : public BuildComplexAction
{
	Vector2 Pos;

public:
	BuildComplexAtStaticAction(S4Api* s4, std::deque<IAction*>* actions, std::vector<BuildEntry> buildings, const Vector2& pos)
		: BuildComplexAction(s4, actions, buildings),
		Pos(pos)
	{}

	virtual bool GetPosition(Vector2& pos) noexcept override
	{
		pos = Pos;
		return true;
	}
};
