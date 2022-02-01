#pragma once

#include <chrono>

#include "../../../../logging/AyyLog.hpp"

#include "../../../../sdk/S4Api.hpp"

#include "../IBuildEngine.hpp"
#include "plans/IBuildPlan.hpp"

constexpr auto BUILD_ENGINE_COOLDOWN = 1000;

class BuildPlanBuildEngine : public IBuildEngine
{
	S4Api* S4;
	IBuildPlan* BuildPlan;

	std::chrono::steady_clock::time_point LastBuildEvent;

public:
	BuildPlanBuildEngine(S4Api* s4, IBuildPlan* buildPlan) noexcept;

	virtual void Tick() noexcept override;
};
