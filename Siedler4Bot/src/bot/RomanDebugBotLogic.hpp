#pragma once

#include "../logging/AyyLog.hpp"

#include "IBotLogic.hpp"

/// <summary>
/// Debug logic that should work for Romans.
/// </summary>
class RomanDebugBotLogic : public IBotLogic
{
public:
	void Tick() noexcept override;
};
