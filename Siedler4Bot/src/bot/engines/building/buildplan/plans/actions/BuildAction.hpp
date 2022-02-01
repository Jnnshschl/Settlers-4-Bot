#pragma once

#include "IAction.hpp"
#include "../../../../../../sdk/S4Api.hpp"

class BuildAction : public IAction
{
protected:
	bool Done;
	S4Api* S4;

public:
	BuildAction(S4Api* s4)
		: S4(s4),
		Done(false)
	{}

	virtual bool Execute() noexcept = 0;

	virtual bool IsDone() noexcept override { return Done; }
};
