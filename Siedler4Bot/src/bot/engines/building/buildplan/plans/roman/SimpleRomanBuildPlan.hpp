#pragma once

#include <deque>
#include <tuple>
#include <functional>

#include "../../../../../../logging/AyyLog.hpp"

#include "../../../../../../sdk/S4Api.hpp"

#include "../IBuildPlan.hpp"
#include "../../actions/IAction.hpp"
#include "../../actions/StaticBuildAction.hpp"
#include "../../actions/ExpandTerritoryAction.hpp"
#include "../../actions/SimpleActionResult.hpp"

class SimpleRomanBuildPlan : public IBuildPlan
{
	S4Api* S4;
	std::deque<IAction*> Actions;

public:
	explicit SimpleRomanBuildPlan(S4Api* s4) noexcept;

	virtual bool Tick() noexcept override;

private:
	void Init() noexcept;
	void BuildStoneQuarry(const Vector2& spot, bool buildTower);
	void BuildForestry(const Vector2& spot, bool buildTower);
};
