#pragma once

enum class SimpleActionResult
{
	ALL,
	OK,
	IN_PROGRESS,
	ERROR_NO_SPACE, // no space to build this building
	ERROR_CLAIMED_BY_ENEMY, // no way to expand to this position
	ERROR_CLAIMED_BY_FRIEND, // no way to expand to this position
	ERROR_NO_RESOURCE, // resource not found
};
