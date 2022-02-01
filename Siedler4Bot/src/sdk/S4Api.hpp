#pragma once

#include <chrono>
#include <deque>

#include <d3d9.h>
#include <d3dx9.h>

#include "../logging/AyyLog.hpp"

#include "../utils/AyyModule.hpp"
#include "../utils/Win32.hpp"
#include "../utils/Pattern.hpp"
#include "../utils/Utils.hpp"
#include "../utils/Vector2.hpp"

#include "static/S4Nation.hpp"
#include "static/S4Event.hpp"
#include "static/S4Building.hpp"
#include "static/S4BuildCheckResult.hpp"
#include "landscape/S4WorldTile.hpp"
#include "landscape/S4Resource.hpp"
#include "landscape/S4EcoSector.hpp"
#include "entities/S4Settler.hpp"
#include "entities/S4MovementType.hpp"

#include "../bot/utils/ResourceSpot.hpp"

// Helper macro to search functions using patterns
// it will search 'pattern' inside 'mod' and put
// it into the 'target' variable, if its not found
// it will return false and exit.
#define SearchFnWrapper(pattern, mod, target) target = SearchFn<decltype(target)>(pattern, sizeof(pattern), mod.Base, mod.Size); if(!target){ AyyLog("Failed to find function: ", #target); return false; }

typedef long(__stdcall* tEndScene) (IDirect3DDevice9*);
typedef unsigned long(__fastcall* tShowTextMessage)(const char*, unsigned long, unsigned long, unsigned long, unsigned long);
typedef void(__stdcall* tSendNetEvent)(S4Event* s4Event);
typedef void(__fastcall* tSendLocalEvent)(void* eventEngine, void* unk, S4Event* s4Event);
typedef void(__fastcall* tWorldToScreen)(unsigned int x, unsigned int y, unsigned int* screenX, unsigned int* screenY);
typedef unsigned int(__stdcall* tGenerateBuildPoints)(int playerId, int buildingId, int unk);
typedef unsigned int(__stdcall* tOnMenuChanged)(int menuId);
typedef int(__stdcall* tBuildCheck)(unsigned int x, unsigned int y, unsigned int playerId, int buildingId, int unk);
typedef void(__fastcall* tBuildingFinished)(int unk);

// TODO: put this into a config file
namespace S4Bot
{
	namespace Config
	{
		constexpr std::chrono::milliseconds MAIN_LOOP_SLEEP_TIME(100);
	}

	namespace Keys
	{
		constexpr unsigned char EXIT = 0x23; // END Key
		constexpr unsigned char DEBUG = 0x2E; // DEL Key
		constexpr unsigned char DEBUG_UI = 0x24; // HOME Key
	}

	namespace Offsets
	{
		constexpr auto MAINHWND = 0xD691E8;
		constexpr auto NETEVENT_VTABLE = 0x123BA08;
		constexpr auto MENU_VTABLE = 0x16AA4B4;

		constexpr auto CURRENT_TICK = 0xD82F04; // 0xE66B14
		constexpr auto TIMER_VISIBLE = 0x11F47A4;

		constexpr auto MAP_SIZE = 0xD6921C;

		constexpr auto LOCAL_PLAYER_ID = 0xE9482C;
		constexpr auto CURRENT_NATION = 0x109B610;
		constexpr auto SELECTED_NATION = 0x109C168;

		constexpr auto WORLD_TILE_TABLE = 0xD69220;
		constexpr auto RESOURCE_TABLE = 0x11630E4;
		constexpr auto ENTITY_TABLE = 0xE9BC38;
		constexpr auto ENTITY_POSITION_TABLE = 0x11630DC;
		constexpr auto ECOSECTOR_ID_TABLE = 0xEF1A18;
		constexpr auto ECOSECTOR_TABLE = 0xEF1A20;
		constexpr auto BUILDINGS_TABLE = 0xDAD184; // total buildings built: 0xDAD570 ref in instruction: 0xDAC05C

		constexpr auto CURRENT_SELECTION_START = 0x10865A4;
		constexpr auto CURRENT_SELECTION_END = 0x10865A8;

		constexpr auto RESOURCE_LOG = 0xDAD9B8;
		constexpr auto RESOURCE_WOOD = 0xDAD97C;
		constexpr auto RESOURCE_STONE = 0xDAD9E0;
	}

	namespace Patterns
	{
		constexpr unsigned char ShowTextMsg[] = { 0x55, 0x8B, 0xEC, 0x53, 0x56, 0x57, 0x6A, 0x24, 0x8B, 0xF2, 0x8B, 0xF9 };
		constexpr unsigned char SendNetEvent[] = { 0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, '?', '?', '?', '?', 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x50, 0x83, 0xEC, 0x38, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xEC, 0x53, 0x56, 0x57, 0x50 };
		constexpr unsigned char SendLocalEvent[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x24, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xFC, 0x53, 0x8B, 0x5D, 0x08, 0x56, 0x57, 0x8B, 0xF9, 0x80, 0x7F, 0x14, 0x00, 0x74, 0x49 };
		constexpr unsigned char BuildCheck[] = { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x83, 0xEC, 0x44, 0x53, 0x8B, '?', '?', '?', '?', '?', 0x56, 0x8B, 0x75, 0x08, 0x57, 0x8B, 0x7D, 0x0C, 0x8D, 0x53, 0xE0, 0xC6, 0x44, 0x24, 0x13, 0x00, 0x8D, 0x46, 0xF0, 0x89, 0x5C, 0x24, 0x1C, 0x3B, 0xC2, 0x8D, 0x47, 0xF0, 0x1B, 0xC9, 0x3B, 0xC2, 0x1B, 0xC0, 0x85, 0xC8 };
		constexpr unsigned char WorldToScreen[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x0C, 0x53, 0x56, 0x8B, '?', '?', '?', '?', '?', 0x8B, 0xDA, 0x89, 0x4D, 0xF8, 0x89, 0x75, 0xF4, 0x85, 0xF6, 0x75, 0x10 };
		constexpr unsigned char BuildingFinished[] = { 0x55, 0x8B, 0xEC, 0x6A, '?', 0x68, '?', '?', '?', '?', 0x64, '?', '?', '?', '?', '?', '?', '?', '?', '?', 0xA1, '?', '?', '?', '?', '?', '?', '?', '?', 0x45, '?', 0x64, '?', '?', '?', '?', '?', '?', '?', '?', '?', 0x46, 0x20 };

		// unused at the moment
		constexpr unsigned char GenerateBuildPoints[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x69, 0x45, 0x0C, 0x2C, 0x03, 0x00, 0x00, 0xB9, 0x0A };
		constexpr unsigned char OnMenuChanged[] = { 0x55, 0x8B, 0xEC, 0x57, 0x8B, 0x3D, 0x94, 0x4C, 0xAE, 0x01, 0x85, 0xFF, 0x0F, 0x84, 0x74, 0x01, 0x00, 0x00, 0x83, 0x3D, 0x18, 0x48, 0x91, 0x01, 0x00 };
	}
}

class S4Api
{
public:
	AyyModule MainModule;

	// functions starting with an '_' should be called using the wrapper
	tSendNetEvent _SendNetEvent = nullptr;
	tSendNetEvent _SendLocalEvent = nullptr;
	tShowTextMessage _ShowTextMsg = nullptr;
	tWorldToScreen _WorldToScreen = nullptr;
	tBuildCheck _BuildCheck = nullptr;
	tBuildingFinished _BuildingFinished = nullptr;

	S4Api();

	inline int Log() noexcept { return MainModule.Read<int>(S4Bot::Offsets::RESOURCE_LOG); }
	inline int Wood() noexcept { return MainModule.Read<int>(S4Bot::Offsets::RESOURCE_WOOD); }
	inline int Stone() noexcept { return MainModule.Read<int>(S4Bot::Offsets::RESOURCE_STONE); }

	inline int LocalPlayerId() noexcept
	{
		return MainModule.Read<int>(S4Bot::Offsets::LOCAL_PLAYER_ID);
	}

	inline int CurrentTick() noexcept
	{
		return MainModule.Read<int>(S4Bot::Offsets::CURRENT_TICK);
	}

	inline int MapSize() noexcept
	{
		return MainModule.Read<int>(S4Bot::Offsets::MAP_SIZE);
	}

	inline unsigned short* Selection() noexcept
	{
		return MainModule.Read<unsigned short*>(S4Bot::Offsets::CURRENT_SELECTION_START);
	}

	inline unsigned short SelectionCount() noexcept
	{
		return MainModule.Read<unsigned short*>(S4Bot::Offsets::CURRENT_SELECTION_END) - Selection();
	}

	inline S4WorldTile* Landscape(const Vector2& position) noexcept
	{
		return MainModule.Read<S4WorldTile*>(S4Bot::Offsets::WORLD_TILE_TABLE) + (position.Y * MapSize() + position.X);
	}

	inline S4Resource Resource(const Vector2& position) noexcept
	{
		return MainModule.Read<S4Resource*>(S4Bot::Offsets::RESOURCE_TABLE)[position.Y * MapSize() + position.X];
	}

	inline S4Entity* Entity(unsigned short id) noexcept
	{
		return MainModule.Read<S4Entity*>(S4Bot::Offsets::ENTITY_TABLE + (id * 4));
	}

	inline unsigned short EcoSectorId(const Vector2& position) noexcept
	{
		return MainModule.Read<unsigned short*>(S4Bot::Offsets::ECOSECTOR_ID_TABLE)[position.Y * MapSize() + position.X];
	}

	inline S4EcoSector* EcoSector(unsigned short id) noexcept
	{
		return reinterpret_cast<S4EcoSector*>(reinterpret_cast<uintptr_t>(MainModule.Base) + S4Bot::Offsets::ECOSECTOR_TABLE) + id;
	}

	inline S4EcoSector* EcoSector(const Vector2& position) noexcept
	{
		return EcoSector(EcoSectorId(position));
	}

	inline unsigned char LandscapeOwner(const Vector2& position) noexcept
	{
		S4EcoSector* sector = EcoSector(position);
		return sector ? sector->Player() : 0;
	}

	inline S4Nation NationId() noexcept
	{
		return MainModule.Read<S4Nation>(S4Bot::Offsets::CURRENT_NATION);
	}

	inline const char* NationName() noexcept
	{
		switch (NationId())
		{
		case S4Nation::Dark:
			return "Dark";
		case S4Nation::Maya:
			return "Maya";
		case S4Nation::Roman:
			return "Roman";
		case S4Nation::Trojan:
			return "Trojan";
		case S4Nation::Viking:
			return "Viking";
		default:
			return "Unknown";
		}
	}

	inline int BuildingCount(S4Building building) noexcept
	{
		return MainModule.Read<int>(S4Bot::Offsets::BUILDINGS_TABLE + (static_cast<int>(building) * sizeof(int)));
	}

	inline S4BuildCheckResult CanBuild(const Vector2& position, S4Building building) noexcept
	{
		return static_cast<S4BuildCheckResult>(_BuildCheck(position.X, position.Y, LocalPlayerId(), static_cast<int>(building), 0));
	}

	// # wrappers

	/// <summary>
	/// Wrapper to make SendNetEvent calls less cluttered by setting vtable,
	/// tick and player automatically as then you dont need to change them.
	/// </summary>
	/// <param name="id">S4NetEventId id</param>
	/// <param name="param0">First parameter</param>
	/// <param name="param1">Second parameter</param>
	inline void SendNetEvent(S4EventId id, unsigned long param0 = 0, unsigned long param1 = 0, unsigned short* settlers = nullptr, unsigned short settlersSize = 0) noexcept
	{
		S4Event testEvent{};
		testEvent.Id = id;
		testEvent.Params[0] = param0;
		testEvent.Params[1] = param1;
		testEvent.VTable = reinterpret_cast<void*>(S4Bot::Offsets::NETEVENT_VTABLE);
		testEvent.Tick = CurrentTick();
		testEvent.Player = LocalPlayerId();
		testEvent.Settlers = settlers;
		testEvent.SettlersSize = settlersSize;
		_SendNetEvent(&testEvent);
	}

	/// <summary>
	/// Wrapper to send main menu navigation events.
	/// </summary>
	/// <param name="id">S4NetEventId id</param>
	inline void SendLocalEventMainMenu(S4EventId id) noexcept
	{
		S4Event testEvent{};
		testEvent.Id = id;
		testEvent.VTable = reinterpret_cast<void*>(S4Bot::Offsets::MENU_VTABLE);
		_SendLocalEvent(&testEvent);
	}

	inline void ShowTextMsg(const char* text, unsigned long icon, unsigned long unk0, unsigned long unk1, unsigned long unk2) noexcept
	{
		_ShowTextMsg(text, icon, unk0, unk1, unk2);
		__asm add esp, 0xC;
	}

	inline void WorldToScreen(const Vector2& position, unsigned int* screenX, unsigned int* screenY) noexcept
	{
		_WorldToScreen(position.X, position.Y, screenX, screenY);
		__asm add esp, 0x8;
	}

	/// <summary>
	/// Build a building at the given position.
	/// </summary>
	/// <param name="building">S4Building to build</param>
	/// <param name="position">Vector2</param>
	inline void Build(S4Building building, const Vector2& position) noexcept
	{
		SendNetEvent(S4EventId::BuildingBuild, static_cast<unsigned long>(building), position.Packed());
	}

	/// <summary>
	/// Build a building at the given position.
	/// </summary>
	/// <param name="building">S4Building to build</param>
	/// <param name="position">Vector2</param>
	inline void BuildAtClosestSpot(S4Building building, const Vector2& position) noexcept
	{
		SendNetEvent(S4EventId::BuildingBuild, static_cast<unsigned long>(building), FindClosestBuildingSpot(position, building).Packed());
	}

	/// <summary>
	/// Move settlers the given position.
	/// </summary>
	/// <param name="settlers">Settlers to move (MAKE SURE THIS ARRAY IS ALLOCATED ON THE HEAP, THE GAME WILL FREE IT!)</param>
	/// <param name="settlersCount">Amount of settlers to move</param>
	/// <param name="type">Movement type</param>
	/// <param name="position">Vector2</param>
	inline void MoveSettlers(unsigned short* settlers, unsigned short settlersCount, const Vector2& position, S4MovementType type = S4MovementType::Normal) noexcept
	{
		SendNetEvent(S4EventId::SettlersMove, position.Packed(), static_cast<unsigned short>(type), settlers, settlersCount * sizeof(unsigned short));
	}

	inline void RecruitCivil(S4Settler type, unsigned long amount, unsigned short ecoSector = 0) noexcept
	{
		SendNetEvent(S4EventId::RecruitCivil, PackShorts(ecoSector, static_cast<unsigned short>(type)), amount);
	}

	inline void RecruitSpecial(S4Settler type, unsigned long amount, unsigned short ecoSector = 0) noexcept
	{
		SendNetEvent(S4EventId::RecruitSpecial, PackShorts(ecoSector, static_cast<unsigned short>(type)), amount);
	}

	inline void RecruitMilitary(S4Settler type, unsigned long amount, unsigned short building) noexcept
	{
		SendNetEvent(S4EventId::RecruitMilitary, PackShorts(building, static_cast<unsigned short>(type)), amount);
	}

	bool Init() noexcept;

	/// <summary>
	/// Try to build resource spots. This is used to determine where 
	/// we should place stuff like stonequarries or woodcutters.
	/// </summary>
	/// <param name="start">Start Resource, example: Stone1</param>
	/// <param name="end">End Resource, example: Stone16</param>
	/// <returns>ResourceSpots found</returns>
	std::vector<ResourceSpot> FindResourceSpots(S4Resource start, S4Resource end) noexcept;

	/// <summary>
	/// Try to find the closest Spot for the building to the given position.
	/// </summary>
	/// <param name="destination">Wanted position</param>
	/// <param name="building">Building to build</param>
	/// <returns>Closest position</returns>
	Vector2 FindClosestBuildingSpot(const Vector2& destination, S4Building building) noexcept;

	/// <summary>
	/// Try to find the closest Spot on the settlement border.
	/// Used to find good positions for towers to expand.
	/// </summary>
	/// <param name="destination">Wanted position</param>
	/// <returns>Closest position</returns>
	Vector2 FindClosestSpotOnBorder(const Vector2& destination) noexcept;
};
