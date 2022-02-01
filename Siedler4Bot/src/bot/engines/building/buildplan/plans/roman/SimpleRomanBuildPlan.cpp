#include "SimpleRomanBuildPlan.hpp"
#include "../actions/StaticBuildAction.hpp"

SimpleRomanBuildPlan::SimpleRomanBuildPlan(S4Api* s4) noexcept
	: S4(s4),
	BuildQueue()
{
	Init();
}

bool SimpleRomanBuildPlan::Tick() noexcept
{
	if (!BuildQueue.empty())
	{
		IAction* q = BuildQueue.front();
		const bool result = q->Execute();

		if (q->IsDone())
		{
			delete q;
			BuildQueue.pop_front();
		}

		return result;
	}

	return false;
}

void SimpleRomanBuildPlan::Init() noexcept
{
	// # STEP 1: claim more stone and wood territory, and harvest them
	const auto& initialStonefields = S4->FindResourceSpots(S4Resource::Stone1, S4Resource::Stone16);
	const auto& initialForests = S4->FindResourceSpots(S4Resource::Wood, S4Resource::Wood);

	if (!initialStonefields.empty())
	{
		const auto& stonefieldCenter = initialStonefields[0].CenterPosition();
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::SmallTower, S4->FindClosestSpotOnBorder(stonefieldCenter)));

		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Stonequarry, stonefieldCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Stonequarry, stonefieldCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Stonequarry, stonefieldCenter));
	}
	else
	{
		// TODO: handle no stonefield case, this is a big problem
	}

	if (!initialForests.empty())
	{
		const auto& forestCenter = initialForests[0].CenterPosition();
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::SmallTower, S4->FindClosestSpotOnBorder(forestCenter)));

		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Woodplanter, forestCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Woodcutter, forestCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Woodcutter, forestCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Woodplanter, forestCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Woodcutter, forestCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Sawmill, forestCenter));
		BuildQueue.push_back(new StaticBuildAction(S4, S4Building::Sawmill, forestCenter));
	}
	else
	{
		// TODO: handle no forest case
	}
}
