#include "RomanSimpleBotLogic.hpp"

RomanSimpleBotLogic::RomanSimpleBotLogic(S4Api* s4) noexcept
	: S4(s4)
{
	BuildPlan = new SimpleRomanBuildPlan(s4);
	BuildEngine = new BuildPlanBuildEngine(s4, BuildPlan);
}

RomanSimpleBotLogic::~RomanSimpleBotLogic() noexcept
{
	delete BuildPlan;
	delete BuildEngine;
}

void RomanSimpleBotLogic::Tick() noexcept
{
	if (BuildEngine)
	{
		BuildEngine->Tick();
	}
}
