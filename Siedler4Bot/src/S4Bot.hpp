#pragma once

#include "logging/AyyLog.hpp"

#include "utils/Win32.hpp"
#include "utils/Pattern.hpp"
#include "utils/Utils.hpp"
#include "utils/S4Position.hpp"

#include "sdk/S4Nation.hpp"
#include "sdk/S4Event.hpp"
#include "sdk/S4Building.hpp"
#include "sdk/S4BuildCheckResult.hpp"
#include "sdk/Landscape/S4WorldTile.hpp"
#include "sdk/Landscape/S4Resource.hpp"
#include "sdk/Landscape/S4EcoSector.hpp"
#include "sdk/Entities/S4Settler.hpp"
#include "sdk/Entities/S4MovementType.hpp"

// Helper macro to search functions using patterns
// it will search 'pattern' inside 'mod' and put
// it into the 'target' variable, if its not found
// it will return false and exit.
#define SearchFnWrapper(pattern, mod, target) target = SearchFn<decltype(target)>(pattern, sizeof(pattern), mod.Base, mod.Size); if(!target){ AyyLog("Failed to find function: ", #target); return false; }

typedef unsigned long(__fastcall* tShowTextMessage)(const char*, unsigned long, unsigned long, unsigned long, unsigned long);
typedef void(__stdcall* tSendNetEvent)(S4Event* s4Event);
typedef void(__fastcall* tSendLocalEvent)(void* eventEngine, void* unk, S4Event* s4Event);
typedef void(__fastcall* tWorldToScreen)(unsigned int x, unsigned int y, unsigned int* screenX, unsigned int* screenY);
typedef unsigned int(__stdcall* tGenerateBuildPoints)(int playerId, int buildingId, int unk);
typedef unsigned int(__stdcall* tOnMenuChanged)(int menuId);
typedef int(__stdcall* tBuildCheck)(unsigned int x, unsigned int y, unsigned int playerId, int buildingId, int unk);

struct AyyModule
{
	const char* Name;
	void* Base;
	unsigned int Size;

	AyyModule(const char* name)
		: Name(name),
		Base(nullptr),
		Size(0)
	{}

	bool Init() noexcept
	{
		return AyyGetModuleHandle(Name, &Base, &Size);
	}

	template<typename T>
	constexpr T Read(ptrdiff_t offset = 0) noexcept
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(Base) + offset);
	}
};

struct ResourceSpot
{
	std::vector<S4Position> Spots{};

	ResourceSpot(S4Position firstResourcePos)
	{
		Spots.push_back(firstResourcePos);
	}

	constexpr auto Amount() const noexcept { return Spots.size(); }

	bool IsNearPool(const S4Position& pos, unsigned short threshold = 16) const noexcept
	{
		for (const auto& spot : Spots)
		{
			if (pos.DistanceTo(spot) < threshold)
			{
				return true;
			}
		}

		return false;
	}

	S4Position CenterPosition() const noexcept
	{
		unsigned short xSum = 0;
		unsigned short ySum = 0;

		for (const auto& spot : Spots)
		{
			xSum += spot.X;
			ySum += spot.Y;
		}

		const unsigned short count = static_cast<unsigned short>(Spots.size());
		return { static_cast<unsigned short>(xSum / count), static_cast<unsigned short>(ySum / count) };
	}
};

namespace S4
{
	inline AyyModule S4MainModule("S4_MAIN.EXE");

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

		// unused at the moment
		constexpr unsigned char GenerateBuildPoints[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x69, 0x45, 0x0C, 0x2C, 0x03, 0x00, 0x00, 0xB9, 0x0A };
		constexpr unsigned char OnMenuChanged[] = { 0x55, 0x8B, 0xEC, 0x57, 0x8B, 0x3D, 0x94, 0x4C, 0xAE, 0x01, 0x85, 0xFF, 0x0F, 0x84, 0x74, 0x01, 0x00, 0x00, 0x83, 0x3D, 0x18, 0x48, 0x91, 0x01, 0x00 };
	}

	namespace Resources
	{
		inline int Log() noexcept { return S4MainModule.Read<int>(Offsets::RESOURCE_LOG); }
		inline int Wood() noexcept { return S4MainModule.Read<int>(Offsets::RESOURCE_WOOD); }
		inline int Stone() noexcept { return S4MainModule.Read<int>(Offsets::RESOURCE_STONE); }
	}

	namespace Game
	{
		// # settlers 4 functions and globals, will initialized in S4Bot::Init()
		tGenerateBuildPoints GenerateBuildPoints = nullptr;
		tOnMenuChanged OnMenuChanged = nullptr;
		// functions starting with an '_' should be called using the wrapper
		tSendNetEvent _SendNetEvent = nullptr;
		tSendNetEvent _SendLocalEvent = nullptr;
		tShowTextMessage _ShowTextMsg = nullptr;
		tWorldToScreen _WorldToScreen = nullptr;
		tBuildCheck _BuildCheck = nullptr;

		// # game interface
		inline int LocalPlayerId() noexcept
		{
			return S4MainModule.Read<int>(Offsets::LOCAL_PLAYER_ID);
		}

		inline int CurrentTick() noexcept
		{
			return S4MainModule.Read<int>(Offsets::CURRENT_TICK);
		}

		inline int MapSize() noexcept
		{
			return S4MainModule.Read<int>(Offsets::MAP_SIZE);
		}

		inline unsigned short* Selection() noexcept
		{
			return S4MainModule.Read<unsigned short*>(Offsets::CURRENT_SELECTION_START);
		}

		inline unsigned short SelectionCount() noexcept
		{
			return S4MainModule.Read<unsigned short*>(Offsets::CURRENT_SELECTION_END) - Selection();
		}

		inline S4WorldTile* Landscape(const S4Position& position) noexcept
		{
			return S4MainModule.Read<S4WorldTile*>(Offsets::WORLD_TILE_TABLE) + (position.Y * MapSize() + position.X);
		}

		inline S4Resource Resource(const S4Position& position) noexcept
		{
			return S4MainModule.Read<S4Resource*>(Offsets::RESOURCE_TABLE)[position.Y * MapSize() + position.X];
		}

		inline S4Entity* Entity(unsigned short id) noexcept
		{
			return S4MainModule.Read<S4Entity*>(Offsets::ENTITY_TABLE + (id * 4));
		}

		inline unsigned short EcoSectorId(const S4Position& position) noexcept
		{
			return S4MainModule.Read<unsigned short*>(Offsets::ECOSECTOR_ID_TABLE)[position.Y * MapSize() + position.X];
		}

		inline S4EcoSector* EcoSector(unsigned short id) noexcept
		{
			return reinterpret_cast<S4EcoSector*>(reinterpret_cast<uintptr_t>(S4MainModule.Base) + Offsets::ECOSECTOR_TABLE) + id;
		}

		inline S4EcoSector* EcoSector(const S4Position& position) noexcept
		{
			return EcoSector(EcoSectorId(position));
		}

		inline unsigned char LandscapeOwner(const S4Position& position) noexcept
		{
			S4EcoSector* sector = EcoSector(position);
			return sector ? sector->Player() : 0;
		}

		inline S4Nation NationId() noexcept
		{
			return S4MainModule.Read<S4Nation>(Offsets::CURRENT_NATION);
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

		inline S4BuildCheckResult CanBuild(const S4Position& position, S4Building building) noexcept
		{
			return static_cast<S4BuildCheckResult>(_BuildCheck(position.X, position.Y, LocalPlayerId(), static_cast<int>(building), 0));
		}

		// # wrappers

		/// <summary>
		/// Wrapper to make SendNetEvent calls less cluttered by setting vtable,
		/// tick and player automatically as they you dont need to change them.
		/// </summary>
		/// <param name="id">S4NetEventId id</param>
		/// <param name="param0">First parameter</param>
		/// <param name="param1">Second parameter</param>
		void SendNetEvent(S4EventId id, unsigned long param0 = 0, unsigned long param1 = 0, unsigned short* settlers = nullptr, size_t settlersSize = 0) noexcept
		{
			S4Event testEvent{};
			testEvent.Id = id;
			testEvent.Params[0] = param0;
			testEvent.Params[1] = param1;
			testEvent.VTable = reinterpret_cast<void*>(Offsets::NETEVENT_VTABLE);
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
		void SendLocalEventMainMenu(S4EventId id) noexcept
		{
			S4Event testEvent{};
			testEvent.Id = id;
			testEvent.VTable = reinterpret_cast<void*>(Offsets::MENU_VTABLE);
			_SendLocalEvent(&testEvent);
		}

		void ShowTextMsg(const char* text, unsigned long icon, unsigned long unk0, unsigned long unk1, unsigned long unk2) noexcept
		{
			_ShowTextMsg(text, icon, unk0, unk1, unk2);
			__asm add esp, 0xC;
		}

		void WorldToScreen(const S4Position& position, unsigned int* screenX, unsigned int* screenY) noexcept
		{
			_WorldToScreen(position.X, position.Y, screenX, screenY);
			__asm add esp, 0x8;
		}

		/// <summary>
		/// Try to build resource spots. This is used to determine where 
		/// we should place stuff like stonequarries or woodcutters.
		/// </summary>
		/// <param name="start">Start Resource, example: Stone1</param>
		/// <param name="end">End Resource, example: Stone16</param>
		/// <returns>ResourceSpots found</returns>
		std::vector<ResourceSpot> FindResourceSpots(S4Resource start, S4Resource end) noexcept
		{
			// find all resources that are in our ecosectors
			std::deque<S4Position> resources{};

			for (unsigned short y = 0; y < S4::Game::MapSize(); ++y)
			{
				for (unsigned short x = 0; x < S4::Game::MapSize(); ++x)
				{
					const S4Position pos{ x, y };

					if (S4::Game::LandscapeOwner(pos) == S4::Game::LocalPlayerId())
					{
						const S4Resource r = S4::Game::Resource(pos);

						if (r >= start && r <= end)
						{
							resources.push_back({ x, y });
						}
					}
				}
			}

			// try to build spots
			std::vector<ResourceSpot> spots{};

			do
			{
				bool foundSpot = false;

				// pop resource from queue
				const auto& res = resources.front();
				resources.pop_front();

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
					spots.push_back({ res });
				}
			} while (!resources.empty());

			// sort by amount of resources
			// TODO: account for the size of the resource pile, 2x 16 spot is better that 5x 1
			std::sort(spots.begin(), spots.end(), [](const ResourceSpot& a, const ResourceSpot& b) { return a.Amount() > b.Amount(); });
			return spots;
		}

		/// <summary>
		/// Try to find the closest Spot for the building to the given position.
		/// </summary>
		/// <param name="destination">Wanted position</param>
		/// <param name="building">Building to build</param>
		/// <returns>Closest position</returns>
		S4Position FindClosestBuildingSpot(const S4Position& destination, S4Building building)
		{
			unsigned short bestDistance = 65535;
			S4Position bestPosition{ 65535, 65535 };

			for (unsigned short y = 0; y < S4::Game::MapSize(); ++y)
			{
				for (unsigned short x = 0; x < S4::Game::MapSize(); ++x)
				{
					const S4Position pos{ x, y };

					if (S4::Game::LandscapeOwner(pos) == S4::Game::LocalPlayerId() && S4::Game::CanBuild(pos, building) >= S4BuildCheckResult::Okay)
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

		// # action functions

		/// <summary>
		/// Build a building at the given position.
		/// </summary>
		/// <param name="building">S4Building to build</param>
		/// <param name="position">Position</param>
		void Build(S4Building building, const S4Position& position) noexcept
		{
			SendNetEvent(S4EventId::BuildingBuild, static_cast<unsigned long>(building), position.Packed());
		}

		/// <summary>
		/// Build a building at the given position.
		/// </summary>
		/// <param name="building">S4Building to build</param>
		/// <param name="position">Position</param>
		void BuildAtClosestSpot(S4Building building, const S4Position& position) noexcept
		{
			SendNetEvent(S4EventId::BuildingBuild, static_cast<unsigned long>(building), FindClosestBuildingSpot(position, building).Packed());
		}

		/// <summary>
		/// Move settlers the given position.
		/// </summary>
		/// <param name="settlers">Settlers to move (MAKE SURE THIS ARRAY IS ALLOCATED ON THE HEAP, THE GAME WILL FREE IT!)</param>
		/// <param name="settlersCount">Amount of settlers to move</param>
		/// <param name="type">Movement type</param>
		/// <param name="position">Position</param>
		void MoveSettlers(unsigned short* settlers, size_t settlersCount, const S4Position& position, S4MovementType type = S4MovementType::Normal) noexcept
		{
			SendNetEvent(S4EventId::SettlersMove, position.Packed(), static_cast<unsigned short>(type), settlers, settlersCount * sizeof(unsigned short));
		}

		void RecruitCivil(S4Settler type, unsigned long amount, unsigned short ecoSector = 0) noexcept
		{
			SendNetEvent(S4EventId::RecruitCivil, PackShorts(ecoSector, static_cast<unsigned short>(type)), amount);
		}

		void RecruitSpecial(S4Settler type, unsigned long amount, unsigned short ecoSector = 0) noexcept
		{
			SendNetEvent(S4EventId::RecruitSpecial, PackShorts(ecoSector, static_cast<unsigned short>(type)), amount);
		}

		void RecruitMilitary(S4Settler type, unsigned long amount, unsigned short building) noexcept
		{
			SendNetEvent(S4EventId::RecruitMilitary, PackShorts(building, static_cast<unsigned short>(type)), amount);
		}
	}

	/// <summary>
	/// Initializes all functions and global variables.
	/// </summary>
	/// <returns>Whether the init was successful or not</returns>
	static bool Init() noexcept
	{
		if (!S4MainModule.Init())
		{
			return false;
		}

		SearchFnWrapper(Patterns::BuildCheck, S4MainModule, Game::_BuildCheck);
		SearchFnWrapper(Patterns::SendNetEvent, S4MainModule, Game::_SendNetEvent);
		SearchFnWrapper(Patterns::SendLocalEvent, S4MainModule, Game::_SendLocalEvent);
		SearchFnWrapper(Patterns::ShowTextMsg, S4MainModule, Game::_ShowTextMsg);
		SearchFnWrapper(Patterns::WorldToScreen, S4MainModule, Game::_WorldToScreen);

		return true;
	}
}