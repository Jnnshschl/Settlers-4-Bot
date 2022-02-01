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

std::vector<ResourceSpot> S4Api::FindResourceSpots(S4Resource start, S4Resource end) noexcept
{
	// find all resources that are in our ecosectors
	std::deque<Vector2> resources{};

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (LandscapeOwner(pos) == LocalPlayerId())
			{
				const S4Resource r = Resource(pos);

				if (r >= start && r <= end)
				{
					resources.push_back({ x, y });
				}
			}
		}
	}

	// try to build spots
	std::vector<ResourceSpot> spots{};

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

	// sort by amount of resources
	// TODO: account for the size of the resource pile, 2x 16 spot is better that 5x 1
	std::sort(spots.begin(), spots.end(), [](const auto& a, const auto& b) { return a.Amount() > b.Amount(); });
	return spots;
}

Vector2 S4Api::FindClosestBuildingSpot(const Vector2& destination, S4Building building) noexcept
{
	unsigned short bestDistance = 65535;
	Vector2 bestPosition{ 65535, 65535 };

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (LandscapeOwner(pos) == LocalPlayerId() && CanBuild(pos, building) >= S4BuildCheckResult::Okay)
			{
				unsigned short distance = destination.DistanceTo(pos);

				if (distance < bestDistance)
				{
					bestDistance = distance;
					bestPosition = { x, y };
				}
			}
		}
	}

	return bestPosition;
}

Vector2 S4Api::FindClosestSpotOnBorder(const Vector2& destination) noexcept
{
	unsigned short bestDistance = 65535;
	Vector2 bestPosition{ 65535, 65535 };

	for (unsigned short y = 0; y < MapSize(); ++y)
	{
		for (unsigned short x = 0; x < MapSize(); ++x)
		{
			const Vector2 pos{ x, y };

			if (LandscapeOwner(pos) == LocalPlayerId() && Landscape(pos)->IsBorder())
			{
				unsigned short distance = destination.DistanceTo(pos);

				if (distance < bestDistance)
				{
					bestDistance = distance;
					bestPosition = { x, y };
				}
			}
		}
	}

	return bestPosition;
}
