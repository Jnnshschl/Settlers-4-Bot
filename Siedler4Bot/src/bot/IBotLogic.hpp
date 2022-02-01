#pragma once

class IBotLogic
{
public:
	virtual ~IBotLogic() noexcept {}

	virtual void Tick() noexcept = 0;
};