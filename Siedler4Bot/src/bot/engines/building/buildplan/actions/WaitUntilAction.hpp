#pragma once

#include <functional>

#include "IAction.hpp"
#include "../../../../../sdk/S4Api.hpp"

class WaitUntilAction : public IAction
{
protected:
	bool Done;
	std::function<bool()> Fn;
	std::function<void()> OnDone;

public:
	WaitUntilAction(std::function<bool()> fn, std::function<void()> onDone)
		: Done(false),
		Fn(fn),
		OnDone(onDone)
	{}

	virtual bool Execute() noexcept override
	{
		Done = Fn();

		if (Done && OnDone)
		{
			OnDone();
		}

		return Done;
	}

	virtual bool IsDone() noexcept override { return Done; }
};
