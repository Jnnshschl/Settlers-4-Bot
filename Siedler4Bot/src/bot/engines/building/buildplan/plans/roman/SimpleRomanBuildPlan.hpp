#pragma once

#include <deque>
#include <tuple>
#include <functional>

#include "../../../../../../logging/AyyLog.hpp"

#include "../../../../../../sdk/S4Api.hpp"

#include "../IBuildPlan.hpp"
#include "../actions/IAction.hpp"

class SimpleRomanBuildPlan : public IBuildPlan
{
	S4Api* S4;

	/// <summary>
	/// 1: Amount of buildings to build of that type
	/// 2: Type of building to build
	/// 3: Function that returns where to build
	/// 4: Function that returns true when building should be built, use this for conditions
	/// </summary>
	std::deque<IAction*> BuildQueue;

public:
	explicit SimpleRomanBuildPlan(S4Api* s4) noexcept;

	virtual bool Tick() noexcept override;

private:
	void Init() noexcept;
};
