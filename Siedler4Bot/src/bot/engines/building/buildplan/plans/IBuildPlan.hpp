#pragma once

#include "../../../../../sdk/static/S4Building.hpp"
#include "../../../../../utils/Vector2.hpp"

class IBuildPlan
{
public:
	virtual ~IBuildPlan() {}

	virtual bool Tick() noexcept = 0;
};
