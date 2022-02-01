#include "BuildPlanBuildEngine.hpp"

BuildPlanBuildEngine::BuildPlanBuildEngine(S4Api* s4, IBuildPlan* buildPlan) noexcept
	: S4(s4),
	BuildPlan(buildPlan),
	LastBuildEvent(std::chrono::high_resolution_clock::now())
{
}

void BuildPlanBuildEngine::Tick() noexcept
{
	const auto buildCooldown = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - LastBuildEvent).count();

	if (buildCooldown < BUILD_ENGINE_COOLDOWN)
	{
		// AyyLog("timeToWait: ", timeToWait);
		return;
	}

	if (BuildPlan && BuildPlan->Tick())
	{
		LastBuildEvent = std::chrono::high_resolution_clock::now();
	}
}
