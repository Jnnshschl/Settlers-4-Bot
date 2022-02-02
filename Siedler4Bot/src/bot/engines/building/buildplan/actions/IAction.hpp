#pragma once

class IAction
{
public:
	virtual ~IAction() noexcept {};

	virtual bool Execute() noexcept = 0;
	virtual bool IsDone() noexcept = 0;
};
