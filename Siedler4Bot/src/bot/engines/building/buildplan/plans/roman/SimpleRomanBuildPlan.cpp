#include "SimpleRomanBuildPlan.hpp"
#include "../../actions/StaticBuildAction.hpp"

SimpleRomanBuildPlan::SimpleRomanBuildPlan(S4Api* s4) noexcept
	: S4(s4),
	Actions()
{
	Init();
}

bool SimpleRomanBuildPlan::Tick() noexcept
{
	if (!Actions.empty())
	{
		IAction* q = Actions.front();
		const bool result = q->Execute();

		if (q->IsDone())
		{
			delete q;
			Actions.pop_front();
		}

		return result;
	}

	return false;
}

void SimpleRomanBuildPlan::Init() noexcept
{
	// STEP 1: Find Wood and Stone
	auto forests = S4->FindResourceSpots(S4Resource::Wood, S4Resource::Wood);

	if (!forests.empty())
	{
		BuildForestry(forests[0].CenterPosition(), true);
	}
	else
	{
		Vector2 freePos;

		if (S4->FindBestFreeBuildingSpot(S4Building::Woodcutter, freePos))
		{
			BuildForestry(freePos, false);
		}
	}

	auto stonefields = S4->FindResourceSpots(S4Resource::Stone1, S4Resource::Stone16);

	if (!stonefields.empty())
	{
		BuildStoneQuarry(stonefields[0].CenterPosition(), true);
	}
	else
	{
		// no stonefield found, search for possible stone mines
		auto stonemines = S4->FindResourceSpots(S4Resource::Stonemine1, S4Resource::Stonemine16);

		if (!stonemines.empty())
		{
			// TODO: need to check whether it makes sense to build more than one mine
			const auto& stoneminesCenter = stonemines[0].CenterPosition();
			Actions.push_back(new StaticBuildAction(S4, S4Building::StoneMine, stoneminesCenter, 1, 5));
		}
		else
		{
			// search for stones outside of our territory
			stonefields = S4->FindResourceSpots(S4Resource::Stone1, S4Resource::Stone16, false, true);

			if (!stonefields.empty())
			{
				const auto expandAction = new ExpandTerritoryAction(S4, stonefields[0].CenterPosition());
				const auto buildQuarryFn = [&](SimpleActionResult r, const Vector2& pos) 
				{
					// build a quarry when SimpleActionResult::OK is returned by the expansion action
					auto newStonefields = S4->FindResourceSpots(S4Resource::Stone1, S4Resource::Stone16);

					if (!newStonefields.empty())
					{
						BuildStoneQuarry(newStonefields[0].CenterPosition(), true);
					}
				};

				expandAction->SetResultHandler(SimpleActionResult::OK, buildQuarryFn);
				Actions.push_back(expandAction);
			}
			else
			{
				// search for possible stone mines outside of our territory
				stonemines = S4->FindResourceSpots(S4Resource::Stonemine1, S4Resource::Stonemine16, false, true);

				if (!stonemines.empty())
				{

					const auto expandAction = new ExpandTerritoryAction(S4, stonemines[0].CenterPosition());
					const auto buildQuarryFn = [&](SimpleActionResult r, const Vector2& pos)
					{
						// build a stonemine when SimpleActionResult::OK is returned by the expansion action
						auto newStonemines = S4->FindResourceSpots(S4Resource::Stonemine1, S4Resource::Stonemine16);

						if (!newStonemines.empty())
						{
							Actions.push_back(new StaticBuildAction(S4, S4Building::StoneMine, newStonemines[0].CenterPosition(), 1, 5));
						}
					};

					expandAction->SetResultHandler(SimpleActionResult::OK, buildQuarryFn);
					Actions.push_back(expandAction);
				}
				else
				{
					// no stones near our territory, we died
					// TODO: handle this "no-stone-at-all" case
				}
			}
		}
	}

	// STEP 2: More settlers
	Actions.push_back(new StaticBuildAction(S4, S4Building::MediumHouse, S4->CalculateSettlementCenter(), 2, 512));

	// STEP 3: Find Coal
	// search for possible stone mines outside of our territory
	auto coalfields = S4->FindResourceSpots(S4Resource::Coal1, S4Resource::Coal16, false, true);

	if (!coalfields.empty())
	{
		const auto expandToCoalAction = new ExpandTerritoryAction(S4, coalfields[0].CenterPosition());
		const auto buildCoalMineFn = [&](SimpleActionResult r, const Vector2& pos)
		{
			// build a stonemine when SimpleActionResult::OK is returned by the expansion action
			auto newCoalfields = S4->FindResourceSpots(S4Resource::Stonemine1, S4Resource::Stonemine16);

			int count = 0;

			for (const auto& coal : newCoalfields)
			{
				Actions.push_back(new StaticBuildAction(S4, S4Building::CoalMine, coal.CenterPosition(), 1, 5));

				// build max 4 mines
				if (++count >= 4) break;
			}
		};

		expandToCoalAction->SetResultHandler(SimpleActionResult::OK, buildCoalMineFn);
		Actions.push_back(expandToCoalAction);
	}

	// STEP 4: Make some Bread if we built coal mines
	// auto river = S4->FindResourceSpots(S4Resource::Coal1, S4Resource::Coal16, false, true);
	
}

void SimpleRomanBuildPlan::BuildStoneQuarry(const Vector2& spot, bool buildTower)
{
	if (buildTower)
	{
		Vector2 stonefieldTowerPosition;

		if (S4->FindClosestSpotOnBorder(spot, stonefieldTowerPosition))
		{
			Actions.push_back(new StaticBuildAction(S4, S4Building::SmallTower, stonefieldTowerPosition));
		}
	}

	Actions.push_back(new StaticBuildAction(S4, S4Building::Stonequarry, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Stonequarry, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Stonequarry, spot));
}

void SimpleRomanBuildPlan::BuildForestry(const Vector2& spot, bool buildTower)
{
	if (buildTower)
	{
		Vector2 forestTowerPosition;

		if (S4->FindClosestSpotOnBorder(spot, forestTowerPosition))
		{
			Actions.push_back(new StaticBuildAction(S4, S4Building::SmallTower, forestTowerPosition));
		}
	}

	Actions.push_back(new StaticBuildAction(S4, S4Building::Woodplanter, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Woodcutter, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Woodcutter, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Woodplanter, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Woodcutter, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Sawmill, spot));
	Actions.push_back(new StaticBuildAction(S4, S4Building::Sawmill, spot));
}
