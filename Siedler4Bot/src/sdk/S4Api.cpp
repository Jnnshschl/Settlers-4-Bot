#include "S4Api.hpp"

S4Api::S4Api()
	: MainModule("S4_MAIN.EXE")
{
}

bool S4Api::Init() noexcept
{
	if (!MainModule.Init())
	{
		return false;
	}

	SearchFnWrapper(S4Bot::Patterns::BuildCheck, MainModule, _BuildCheck);
	SearchFnWrapper(S4Bot::Patterns::SendNetEvent, MainModule, _SendNetEvent);
	SearchFnWrapper(S4Bot::Patterns::SendLocalEvent, MainModule, _SendLocalEvent);
	SearchFnWrapper(S4Bot::Patterns::ShowTextMsg, MainModule, _ShowTextMsg);
	SearchFnWrapper(S4Bot::Patterns::WorldToScreen, MainModule, _WorldToScreen);
	SearchFnWrapper(S4Bot::Patterns::BuildingFinished, MainModule, _BuildingFinished);

	return true;
}

std::vector<ResourceSpot> S4Api::FindResourceSpots(S4Resource start, S4Resource end, bool includeOwn, bool includeUnclaimed, bool includeEnemy) noexcept
{
	// find all resources that are in our ecosectors
	std::deque<Vector2> resources{};
	std::vector<ResourceSpot> spots{};

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };
			const auto owner = LandscapeOwner(pos);

			if ((includeOwn && owner == LocalPlayerId())
				|| (includeUnclaimed && owner == 0)
				|| (includeEnemy && owner > 0 && owner != LocalPlayerId())) // TODO: filter friendly players as we cant claim it
			{
				const S4Resource r = Resource(pos);

				if (r >= start && r <= end)
				{
					resources.push_back({ pos.X, pos.Y });
				}
			}
		}
	}

	if (resources.empty()) return spots;

	do
	{
		bool foundSpot = false;

		// pop resource from queue
		const auto& res = resources.front();

		// check wheter it belongs to a spot or not
		for (auto& s : spots)
		{
			if (s.IsNearPool(res))
			{
				s.Spots.push_back(res);
				foundSpot = true;
				break;
			}
		}

		if (!foundSpot)
		{
			// found no spot, create a new one
			spots.push_back(ResourceSpot{ res });
		}

		resources.pop_front();
	} while (!resources.empty());

	return spots;
}

bool S4Api::FindClosestBuildingSpot(const Vector2& destination, S4Building building, Vector2& position, int maxDistance, int minDistance) noexcept
{
	bool found = false;
	unsigned short bestDistance = USHRT_MAX;
	position = Vector2{ USHRT_MAX, USHRT_MAX };

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (LandscapeOwner(pos) == LocalPlayerId() && CanBuild(pos, building) >= S4BuildCheckResult::Okay)
			{
				unsigned short distance = destination.DistanceTo(pos);

				if (distance >= minDistance && distance <= maxDistance && distance < bestDistance)
				{
					bestDistance = distance;
					position = pos;
					found = true;
				}
			}
		}
	}

	return found;
}

bool S4Api::FindClosestSpotOnBorder(const Vector2& destination, Vector2& position) noexcept
{
	bool found = false;
	unsigned short bestDistance = USHRT_MAX;
	position = Vector2{ USHRT_MAX, USHRT_MAX };

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (Landscape(pos)->IsBorder() && LandscapeOwner(pos) == LocalPlayerId())
			{
				unsigned short distance = destination.DistanceTo(pos);

				if (distance < bestDistance)
				{
					bestDistance = distance;
					position = pos;
					found = true;
				}
			}
		}
	}

	return found;
}

bool S4Api::FindClosestTerrain(const Vector2& destination, S4GroundType start, S4GroundType end, Vector2& position, bool includeOwn, bool includeUnclaimed, bool includeEnemy) noexcept
{
	bool found = false;
	unsigned short bestDistance = USHRT_MAX;
	position = Vector2{ USHRT_MAX, USHRT_MAX };

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };
			const auto owner = LandscapeOwner(pos);

			if ((includeOwn && owner == LocalPlayerId())
				|| (includeUnclaimed && owner == 0)
				|| (includeEnemy && owner > 0 && owner != LocalPlayerId())) // TODO: filter friendly players as we cant claim it
			{
				if (Landscape(pos)->Type >= start && Landscape(pos)->Type <= end)
				{
					unsigned short distance = destination.DistanceTo(pos);

					if (distance < bestDistance)
					{
						bestDistance = distance;
						position = pos;
						found = true;
					}
				}
			}
		}
	}

	return found;
}

bool S4Api::FindBestFreeBuildingSpot(S4Building building, Vector2& position) noexcept
{
	bool found = false;
	Vector2 avgPos;
	int count = 0;

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (LandscapeOwner(pos) == LocalPlayerId() && CanBuild(pos, building) >= S4BuildCheckResult::Okay)
			{
				avgPos += pos;
				++count;
			}
		}
	}

	avgPos /= count;

	if (FindClosestBuildingSpot(avgPos, building, position))
	{
		found = true;
	}

	return found;
}

S4LandscapeStatus S4Api::GetTerrainStatus(const Vector2& position) noexcept
{
	const auto owner = LandscapeOwner(position);

	if (owner == LocalPlayerId())
	{
		return S4LandscapeStatus::OWN;
	}
	else if (owner > 0)
	{
		// TODO: recognize friendly players
		return S4LandscapeStatus::ENEMY;
	}

	return S4LandscapeStatus::UNCLAIMED;
}

Vector2 S4Api::CalculateSettlementCenter() noexcept
{
	int avgX = 0;
	int avgY = 0;
	int count = 0;

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (LandscapeOwner(pos) == LocalPlayerId())
			{
				avgX += x;
				avgY += y;
				++count;
			}
		}
	}

	avgX /= count;
	avgY /= count;
	return { static_cast<unsigned short>(avgX), static_cast<unsigned short>(avgY) };
}
