#pragma once

#include <vector>
#include <functional>

#include "IAction.hpp"
#include "SimpleActionResult.hpp"
#include "../../../../../sdk/S4Api.hpp"

class SimpleAction : public IAction
{
protected:
	bool Done;
	S4Api* S4;

	std::vector<std::pair<SimpleActionResult, std::function<void(SimpleActionResult, const Vector2&)>>> ResultHandlers;

public:
	SimpleAction(S4Api* s4)
		: S4(s4),
		Done(false),
		ResultHandlers()
	{}

	virtual bool Execute() noexcept = 0;

	virtual bool IsDone() noexcept override { return Done; }

	void CallResultHandlers(SimpleActionResult result, const Vector2& pos) noexcept
	{
		if (!ResultHandlers.empty())
		{
			for (const auto& resultHandler : ResultHandlers)
			{
				if ((resultHandler.first == result || resultHandler.first == SimpleActionResult::ALL) && resultHandler.second)
				{
					resultHandler.second(result, pos);
				}
			}
		}
	}

	void AddResultHandler(SimpleActionResult result, std::function<void(SimpleActionResult, const Vector2&)> resultHandler) noexcept
	{
		ResultHandlers.push_back(std::make_pair(result, resultHandler));
	}

	void ClearResultHandlers() noexcept
	{
		ResultHandlers.clear();
	}
};
