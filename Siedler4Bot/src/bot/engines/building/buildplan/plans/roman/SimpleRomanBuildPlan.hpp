#pragma once

#include <deque>
#include <tuple>
#include <functional>

#include "../../../../../../logging/AyyLog.hpp"

#include "../../../../../../sdk/S4Api.hpp"

#include "../IBuildPlan.hpp"
#include "../../actions/IAction.hpp"
#include "../../actions/WaitUntilAction.hpp"
#include "../../actions/StaticBuildAction.hpp"
#include "../../actions/SimpleActionResult.hpp"
#include "../../actions/ExpandTerritoryAction.hpp"
#include "../../actions/complex/BuildComplexAtStaticAction.hpp"
#include "../../actions/complex/BuildComplexAtResourcesAction.hpp"
#include "../../actions/complex/BuildComplexAtLandscapeAction.hpp"

class SimpleRomanBuildPlan : public IBuildPlan
{
	S4Api* S4;
	std::deque<IAction*> Actions;

public:
	explicit SimpleRomanBuildPlan(S4Api* s4) noexcept;

	virtual bool Tick() noexcept override;

private:
	void Init() noexcept;

	void BuildForestry(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildForestryTower(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildStoneQuarry(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildStoneMine(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildStoneTower(SimpleActionResult r, const Vector2& pos) noexcept;
	void ExpandAndBuildStoneQuarry(SimpleActionResult r, const Vector2& pos) noexcept;
	void ExpandAndBuildStoneMine(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildMediumHouses(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildCoalMine(SimpleActionResult r, const Vector2& pos) noexcept;
	void ExpandAndBuildCoalMine(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildWaterworks(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildBreadProduction(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildIronMine(SimpleActionResult r, const Vector2& pos) noexcept;
	void ExpandAndBuildIronMine(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildMeatProduction(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildGoldMine(SimpleActionResult r, const Vector2& pos) noexcept;
	void ExpandAndBuildGoldMine(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildFishingHuts(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildIronSmeltery(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildGoldSmeltery(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildMediumHouses2(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildWeaponSmith(SimpleActionResult r, const Vector2& pos) noexcept;
	void BuildToolSmith(SimpleActionResult r, const Vector2& pos) noexcept;

	void BuildBarracks(SimpleActionResult r, const Vector2& pos) noexcept;
};
