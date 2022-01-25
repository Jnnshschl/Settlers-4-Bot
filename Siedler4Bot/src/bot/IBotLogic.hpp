#pragma once

class IBotLogic
{
public:
	virtual void Tick() noexcept = 0;
};