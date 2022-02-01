#pragma once

class IBuildEngine
{
public:
	virtual ~IBuildEngine() noexcept {}

	virtual void Tick() noexcept = 0;
};
