#pragma once

#include "../../../logging/AyyLog.hpp"

#include "../../../sdk/S4Api.hpp"

#include "../../IBotLogic.hpp"
#include "../../engines/building/IBuildEngine.hpp"
#include "../../engines/building/buildplan/plans/IBuildPlan.hpp"

#include "../../engines/building/buildplan/BuildPlanBuildEngine.hpp"
#include "../../engines/building/buildplan/plans/roman/SimpleRomanBuildPlan.hpp"

/// <summary>
/// Debug logic that should work for Romans.
/// </summary>
class RomanSimpleBotLogic : public IBotLogic
{
	S4Api* S4;
	IBuildPlan* BuildPlan;
	IBuildEngine* BuildEngine;

public:
	explicit RomanSimpleBotLogic(S4Api* s4) noexcept;
	~RomanSimpleBotLogic() noexcept;

	RomanSimpleBotLogic(const RomanSimpleBotLogic&) = delete;
	RomanSimpleBotLogic& operator=(const RomanSimpleBotLogic&) = delete;

	void Tick() noexcept override;
};
