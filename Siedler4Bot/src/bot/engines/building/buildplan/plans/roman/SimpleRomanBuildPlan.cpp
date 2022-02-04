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
	BuildForestry(SimpleActionResult::ALL, Vector2());

	// auto stonefields = S4->FindResourceSpotsSortByAmount(S4Resource::Stone1, S4Resource::Stone16);
	// 
	// if (!stonefields.empty())
	// {
	// 	BuildStoneQuarry(stonefields[0].CenterPosition(), true);
	// }
	// else
	// {
	// 	// no stonefield found, search for possible stone mines
	// 	auto stonemines = S4->FindResourceSpotsSortByAmount(S4Resource::Stonemine1, S4Resource::Stonemine16);
	// 
	// 	if (!stonemines.empty())
	// 	{
	// 		// TODO: need to check whether it makes sense to build more than one mine
	// 		Actions.push_back(new StaticBuildAction(S4, S4Building::StoneMine, stonemines[0].CenterPosition(), 2, 5));
	// 	}
	// 	else
	// 	{
	// 		// search for stones outside of our territory
	// 		auto stonefieldsOutside = S4->FindResourceSpotsSortByAmount(S4Resource::Stone1, S4Resource::Stone16, false, true);
	// 
	// 		if (!stonefieldsOutside.empty())
	// 		{
	// 			const auto expandAction = new ExpandTerritoryAction(S4, stonefieldsOutside[0].CenterPosition());
	// 			const auto buildQuarryFn = [&](SimpleActionResult r, const Vector2& pos)
	// 			{
	// 				// build a quarry when SimpleActionResult::OK is returned by the expansion action
	// 				auto newStonefields = S4->FindResourceSpotsSortByAmount(S4Resource::Stone1, S4Resource::Stone16);
	// 
	// 				if (!newStonefields.empty())
	// 				{
	// 					BuildStoneQuarry(newStonefields[0].CenterPosition(), true);
	// 				}
	// 			};
	// 
	// 			expandAction->SetResultHandler(SimpleActionResult::OK, buildQuarryFn);
	// 			Actions.push_back(expandAction);
	// 		}
	// 		else
	// 		{
	// 			// search for possible stone mines outside of our territory
	// 			auto stoneminesOutside = S4->FindResourceSpotsSortByAmount(S4Resource::Stonemine1, S4Resource::Stonemine16, false, true);
	// 
	// 			if (!stoneminesOutside.empty())
	// 			{
	// 				const auto expandAction = new ExpandTerritoryAction(S4, stoneminesOutside[0].CenterPosition());
	// 				const auto buildQuarryFn = [&](SimpleActionResult r, const Vector2& pos)
	// 				{
	// 					// build a stonemine when SimpleActionResult::OK is returned by the expansion action
	// 					auto newStonemines = S4->FindResourceSpotsSortByAmount(S4Resource::Stonemine1, S4Resource::Stonemine16);
	// 
	// 					if (!newStonemines.empty())
	// 					{
	// 						Actions.push_back(new StaticBuildAction(S4, S4Building::StoneMine, newStonemines[0].CenterPosition(), 1, 5));
	// 					}
	// 				};
	// 
	// 				expandAction->SetResultHandler(SimpleActionResult::OK, buildQuarryFn);
	// 				Actions.push_back(expandAction);
	// 			}
	// 			else
	// 			{
	// 				// no stones near our territory, we died
	// 				// TODO: handle this "no-stone-at-all" case
	// 			}
	// 		}
	// 	}
	// }
	// 
	// // STEP 2: More settlers
	// Actions.push_back(new StaticBuildAction(S4, S4Building::MediumHouse, S4->CalculateSettlementCenter(), 2, 512));
	// 
	// // STEP 3: Find Coal
	// // search for possible coal mines in our territory
	// auto settlementCenter = S4->CalculateSettlementCenter();
	// auto coalfields = S4->FindResourceSpotsSortByDistance(settlementCenter, S4Resource::Coal1, S4Resource::Coal16);
	// 
	// if (!coalfields.empty())
	// {
	// 	Actions.push_back(new StaticBuildAction(S4, S4Building::CoalMine, coalfields[0].CenterPosition(), 3, 5));
	// }
	// else
	// {
	// 	auto coalfieldsOutside = S4->FindResourceSpotsSortByDistance(settlementCenter, S4Resource::Coal1, S4Resource::Coal16, false, true);
	// 
	// 	if (!coalfieldsOutside.empty())
	// 	{
	// 		const auto expandToCoalAction = new ExpandTerritoryAction(S4, coalfieldsOutside[0].CenterPosition());
	// 		const auto buildCoalMineFn = [&](SimpleActionResult r, const Vector2& pos)
	// 		{
	// 			// build a coalmine when SimpleActionResult::OK is returned by the expansion action
	// 			int count = 0;
	// 
	// 			for (const auto& coal : S4->FindResourceSpotsSortByDistance(settlementCenter, S4Resource::Coal1, S4Resource::Coal16))
	// 			{
	// 				if (++count >= 2) break;
	// 				Actions.push_back(new StaticBuildAction(S4, S4Building::CoalMine, coal.CenterPosition(), 2, 5));
	// 			}
	// 		};
	// 
	// 		expandToCoalAction->SetResultHandler(SimpleActionResult::OK, buildCoalMineFn);
	// 		Actions.push_back(expandToCoalAction);
	// 	}
	// }
	// 
	// // STEP 4: Make some Bread if we built coal mines
	// Vector2 riverPos;
	// 
	// if (S4->FindClosestTerrain(settlementCenter, S4GroundType::River1, S4GroundType::River4, riverPos))
	// {
	// 	Actions.push_back(new StaticBuildAction(S4, S4Building::Waterwork, riverPos, 2, 20));
	// 	Actions.push_back(new StaticBuildAction(S4, S4Building::Windmill, riverPos, 2, 80));
	// 	Actions.push_back(new StaticBuildAction(S4, S4Building::Bakery, riverPos, 2, 100));
	// 	Actions.push_back(new StaticBuildAction(S4, S4Building::Farm, riverPos, 2, 150));
	// }
}

void SimpleRomanBuildPlan::BuildForestry(SimpleActionResult r, const Vector2& pos) noexcept
{
	std::vector<BuildEntry> forestryBuildings =
	{
		{ S4Building::SmallTower, 1 },
		{ S4Building::Woodplanter, 1 },
		{ S4Building::Woodcutter, 3 },
		{ S4Building::Woodplanter, 1 },
		{ S4Building::Sawmill, 2 }
	};

	auto action = new BuildComplexAtResourcesAction(S4, &Actions, forestryBuildings, S4Resource::Wood, S4Resource::Wood);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildForestryTower(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildForestryTower(SimpleActionResult r, const Vector2& pos) noexcept
{
	Vector2 borderPos;

	if (S4->FindClosestSpotOnBorder(pos, borderPos))
	{
		auto action = new StaticBuildAction(S4, S4Building::SmallTower, borderPos);
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildStoneQuarry(args...); });
		Actions.push_back(action);
	}
	else
	{
		BuildStoneQuarry(SimpleActionResult::OK, Vector2());
	}
}

void SimpleRomanBuildPlan::BuildStoneQuarry(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Stonequarry, 3 } }, S4Resource::Stone1, S4Resource::Stone16);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildStoneTower(args...); });
	action->AddResultHandler(SimpleActionResult::ERROR_NO_RESOURCE, [=](auto && ...args) { return BuildStoneMine(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildStoneMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::StoneMine, 2, 0, 2 } }, S4Resource::Stonemine1, S4Resource::Stonemine16);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildStoneTower(args...); });
	action->AddResultHandler(SimpleActionResult::ERROR_NO_RESOURCE, [=](auto && ...args) { return ExpandAndBuildStoneQuarry(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildStoneTower(SimpleActionResult r, const Vector2& pos) noexcept
{
	Vector2 borderPos;

	if (S4->FindClosestSpotOnBorder(pos, borderPos))
	{
		auto action = new StaticBuildAction(S4, S4Building::SmallTower, borderPos);
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildMediumHouses(args...); });
		Actions.push_back(action);
	}
	else
	{
		BuildMediumHouses(SimpleActionResult::OK, Vector2());
	}
}

void SimpleRomanBuildPlan::ExpandAndBuildStoneQuarry(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto stonequarryOutside = S4->FindResourceSpotsSortByDistance(S4->CalculateSettlementCenter(), S4Resource::Stone1, S4Resource::Stone16, false, true);

	if (!stonequarryOutside.empty())
	{
		const auto action = new ExpandTerritoryAction(S4, stonequarryOutside[0].CenterPosition());
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildStoneQuarry(args...); });
		action->AddResultHandler(SimpleActionResult::ERROR_CLAIMED_BY_ENEMY, [=](auto && ...args) { return ExpandAndBuildStoneMine(args...); });
		action->AddResultHandler(SimpleActionResult::ERROR_CLAIMED_BY_FRIEND, [=](auto && ...args) { return ExpandAndBuildStoneMine(args...); });
		Actions.push_back(action);
	}
	else
	{
		ExpandAndBuildStoneMine(SimpleActionResult::ALL, Vector2());
	}
}

void SimpleRomanBuildPlan::ExpandAndBuildStoneMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto stoneminesOutside = S4->FindResourceSpotsSortByDistance(S4->CalculateSettlementCenter(), S4Resource::Stone1, S4Resource::Stone16, false, true);

	if (!stoneminesOutside.empty())
	{
		const auto action = new ExpandTerritoryAction(S4, stoneminesOutside[0].CenterPosition());
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildStoneMine(args...); });
		Actions.push_back(action);
	}
}

void SimpleRomanBuildPlan::BuildMediumHouses(SimpleActionResult r, const Vector2& pos) noexcept
{
	Vector2 p;

	if (!S4->FindBestFreeBuildingSpot(S4Building::MediumHouse, p))
	{
		p = S4->CalculateSettlementCenter();
	}

	// wait until we finished basic resource production
	auto waitCondition = [&]()
	{
		return S4->BuildingCount(S4Building::Woodcutter) > 0
			&& S4->BuildingCount(S4Building::Woodplanter) > 0
			&& S4->BuildingCount(S4Building::Sawmill) > 0
			&& (S4->BuildingCount(S4Building::Stonequarry) > 0 || S4->BuildingCount(S4Building::StoneMine) > 0);
	};

	auto waitOnDone = [&]()
	{
		auto action = new StaticBuildAction(S4, S4Building::MediumHouse, p, 1);
		action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildCoalMine(args...); });
		Actions.push_back(action);
	};

	auto waitAction = new WaitUntilAction(waitCondition, waitOnDone);
	Actions.push_back(waitAction);
}

void SimpleRomanBuildPlan::BuildCoalMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::CoalMine, 2, 0, 2 } }, S4Resource::Coal1, S4Resource::Coal16);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildWaterworks(args...); });
	action->AddResultHandler(SimpleActionResult::ERROR_NO_RESOURCE, [=](auto && ...args) { return ExpandAndBuildCoalMine(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::ExpandAndBuildCoalMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto minesOutside = S4->FindResourceSpotsSortByDistance(S4->CalculateSettlementCenter(), S4Resource::Coal1, S4Resource::Coal16, false, true);

	if (!minesOutside.empty())
	{
		const auto action = new ExpandTerritoryAction(S4, minesOutside[0].CenterPosition());
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildCoalMine(args...); });
		Actions.push_back(action);
	}
}

void SimpleRomanBuildPlan::BuildWaterworks(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtLandscapeAction(S4, &Actions, { { S4Building::Waterwork, 2 } }, S4GroundType::River1, S4GroundType::River4);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildBreadProduction(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildBreadProduction(SimpleActionResult r, const Vector2& pos) noexcept
{
	std::vector<BuildEntry> farmBuildings =
	{
		{ S4Building::Windmill, 1 },
		{ S4Building::Bakery, 1 },
		{ S4Building::Farm, 1 },
	};

	Vector2 p;

	if (!S4->FindBestFreeBuildingSpot(S4Building::MediumHouse, p))
	{
		p = S4->CalculateSettlementCenter();
	}

	auto action = new BuildComplexAtStaticAction(S4, &Actions, farmBuildings, p);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildIronMine(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildIronMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::IronMine, 2, 0, 2 } }, S4Resource::Iron1, S4Resource::Iron16);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildMeatProduction(args...); });
	action->AddResultHandler(SimpleActionResult::ERROR_NO_RESOURCE, [=](auto && ...args) { return ExpandAndBuildIronMine(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::ExpandAndBuildIronMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto minesOutside = S4->FindResourceSpotsSortByDistance(S4->CalculateSettlementCenter(), S4Resource::Iron1, S4Resource::Iron16, false, true);

	if (!minesOutside.empty())
	{
		const auto action = new ExpandTerritoryAction(S4, minesOutside[0].CenterPosition());
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildIronMine(args...); });
		Actions.push_back(action);
	}
}

void SimpleRomanBuildPlan::BuildMeatProduction(SimpleActionResult r, const Vector2& pos) noexcept
{
	std::vector<BuildEntry> meatBuildings =
	{
		{ S4Building::Pigfarm, 1 },
		{ S4Building::Slaughter, 1 },
	};

	auto action = new BuildComplexAtLandscapeAction(S4, &Actions, meatBuildings, S4GroundType::River1, S4GroundType::River4);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildGoldMine(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildGoldMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::GoldMine, 2, 0, 2 } }, S4Resource::Gold1, S4Resource::Gold16);
	action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildFishingHuts(args...); });
	action->AddResultHandler(SimpleActionResult::ERROR_NO_RESOURCE, [=](auto && ...args) { return ExpandAndBuildGoldMine(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::ExpandAndBuildGoldMine(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto minesOutside = S4->FindResourceSpotsSortByDistance(S4->CalculateSettlementCenter(), S4Resource::Gold1, S4Resource::Gold16, false, true);

	if (!minesOutside.empty())
	{
		const auto action = new ExpandTerritoryAction(S4, minesOutside[0].CenterPosition());
		action->AddResultHandler(SimpleActionResult::OK, [=](auto && ...args) { return BuildGoldMine(args...); });
		Actions.push_back(action);
	}
}

void SimpleRomanBuildPlan::BuildFishingHuts(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Fisher, 1 } }, S4Resource::Fish1, S4Resource::Fish16);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildIronSmeltery(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildIronSmeltery(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Ironsmelter, 2 } }, S4Resource::Iron1, S4Resource::Iron16);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildGoldSmeltery(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildGoldSmeltery(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Goldsmelter, 2 } }, S4Resource::Gold1, S4Resource::Gold16);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildMediumHouses2(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildMediumHouses2(SimpleActionResult r, const Vector2& pos) noexcept
{
	Vector2 p;

	if (!S4->FindBestFreeBuildingSpot(S4Building::MediumHouse, p))
	{
		p = S4->CalculateSettlementCenter();
	}

	auto action = new StaticBuildAction(S4, S4Building::MediumHouse, p, 2);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildWeaponSmith(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildWeaponSmith(SimpleActionResult r, const Vector2& pos) noexcept
{
	AyyLog((int)r);
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Weaponsmith, 2 } }, S4Resource::Iron1, S4Resource::Iron16);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildToolSmith(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildToolSmith(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Toolsmith, 1 } }, S4Resource::Iron1, S4Resource::Iron16);
	action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildBarracks(args...); });
	Actions.push_back(action);
}

void SimpleRomanBuildPlan::BuildBarracks(SimpleActionResult r, const Vector2& pos) noexcept
{
	auto action = new BuildComplexAtResourcesAction(S4, &Actions, { { S4Building::Barracks, 1 } }, S4Resource::Iron1, S4Resource::Iron16);
	// action->AddResultHandler(SimpleActionResult::ALL, [=](auto && ...args) { return BuildBarracks(args...); });
	Actions.push_back(action);
}
