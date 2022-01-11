#pragma once

#include <chrono>

#include "utils/Win32.hpp"
#include "utils/Pattern.hpp"

#include "sdk/S4Nation.hpp"
#include "sdk/S4Event.hpp"
#include "sdk/S4Building.hpp"
#include "sdk/Landscape/S4WorldTile.hpp"
#include "sdk/Landscape/S4Resource.hpp"
#include "sdk/Entities/S4Settler.hpp"
#include "sdk/Entities/S4MovementType.hpp"

// helper macro to search functions using patterns
#define SearchFnWrapper(target, mod, pattern) target = SearchFn<decltype(target)>(pattern, sizeof(pattern), mod.base, mod.size)

typedef unsigned long(__fastcall* tShowTextMessage)(const char*, unsigned long, unsigned long, unsigned long, unsigned long);
typedef void(__stdcall* tSendNetEvent)(S4Event* s4Event);
typedef void(__fastcall* tSendLocalEvent)(void* eventEngine, void* unk, S4Event* s4Event);
typedef void(__fastcall* tWorldToScreen)(unsigned int x, unsigned int y, unsigned int* screenX, unsigned int* screenY);
typedef unsigned int(__stdcall* tGenerateBuildPoints)(int playerId, int buildingId, int unk);

struct AyyModule
{
    const char* name;
    void* base;
    unsigned int size;

    AyyModule(const char* name)
        : name(name),
        base(nullptr),
        size(0)
    {}

    bool Init() noexcept
    {
        return AyyGetModuleHandle(name, &base, &size);
    }

    template<typename T>
    constexpr T Read(ptrdiff_t offset = 0) noexcept
    {
        return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
    }
};

namespace S4Bot
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
        constexpr auto ECOSECTOR_TABLE = 0xEF1A18;

        constexpr auto CURRENT_SELECTION_START = 0x10865A4;
        constexpr auto CURRENT_SELECTION_END = 0x10865A8;
    }

    namespace Patterns
    {
        constexpr unsigned char ShowTextMsg[] = { 0x55, 0x8B, 0xEC, 0x53, 0x56, 0x57, 0x6A, 0x24, 0x8B, 0xF2, 0x8B, 0xF9 };
        constexpr unsigned char SendNetEvent[] = { 0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, '?', '?', '?', '?', 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x50, 0x83, 0xEC, 0x38, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xEC, 0x53, 0x56, 0x57, 0x50 };
        constexpr unsigned char SendLocalEvent[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x24, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xFC, 0x53, 0x8B, 0x5D, 0x08, 0x56, 0x57, 0x8B, 0xF9, 0x80, 0x7F, 0x14, 0x00, 0x74, 0x49 };
        
        // TODO: this is only integer precision, try to improve this to float
        constexpr unsigned char WorldToScreen[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x0C, 0x53, 0x56, 0x8B, '?', '?', '?', '?', '?', 0x8B, 0xDA, 0x89, 0x4D, 0xF8, 0x89, 0x75, 0xF4, 0x85, 0xF6, 0x75, 0x10 };
        
        constexpr unsigned char GenerateBuildPoints[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x69, 0x45, 0x0C, 0x2C, 0x03, 0x00, 0x00, 0xB9, 0x0A };
    }

    namespace Game
    {
        // # settlers 4 functions and globals, will initialized in S4Bot::Init()
        tSendNetEvent SendNetEvent = nullptr;
        tSendNetEvent SendLocalEvent = nullptr;
        tGenerateBuildPoints GenerateBuildPoints = nullptr;
        // functions starting with an '_' should be called using a wrapper
        tShowTextMessage _ShowTextMsg = nullptr;
        tWorldToScreen _WorldToScreen = nullptr;

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

        inline S4WorldTile* LandscapeTable() noexcept
        {
            return S4MainModule.Read<S4WorldTile*>(Offsets::WORLD_TILE_TABLE);
        }

        inline S4WorldTile* Landscape(unsigned short x, unsigned short y) noexcept
        {
            return LandscapeTable() + (y * MapSize() + x);
        }

        inline S4Resource* ResourceTable() noexcept
        {
            return S4MainModule.Read<S4Resource*>(Offsets::RESOURCE_TABLE);
        }

        inline S4Resource Resource(unsigned short x, unsigned short y) noexcept
        {
            return ResourceTable()[y * MapSize() + x];
        }

        inline S4Entity* Entity(unsigned short id) noexcept
        {
            return S4MainModule.Read<S4Entity*>(Offsets::ENTITY_TABLE + (id * 4));
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

        // # wrappers

        /// <summary>
        /// Wrapper to make SendNetEvent calls less cluttered by setting vtable,
        /// tick and player automatically as they you dont need to change them.
        /// </summary>
        /// <param name="id">S4NetEventId id</param>
        /// <param name="param0">First parameter</param>
        /// <param name="param1">Second parameter</param>
        void SendNetEventWrapper(S4EventId id, unsigned long param0, unsigned long param1, unsigned short* settlers = nullptr, size_t settlersSize = 0) noexcept
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
            SendNetEvent(&testEvent);
        }

        void ShowTextMsg(const char* text, unsigned long icon, unsigned long unk0, unsigned long unk1, unsigned long unk2) noexcept
        {
            _ShowTextMsg(text, icon, unk0, unk1, unk2);
            __asm add esp, 0xC;
        }

        void WorldToScreen(unsigned int x, unsigned int y, unsigned int* screenX, unsigned int* screenY) noexcept
        {
            _WorldToScreen(x, y, screenX, screenY);
            __asm add esp, 0x8;
        }

        // # helpers

        /// <summary>
        /// Helper function to pack two unsigned shorts into an unsigned long,
        /// this is used in the S4Event params when a position is set.
        /// </summary>
        /// <param name="x">X coordinate</param>
        /// <param name="y">Y coordinate</param>
        /// <returns>Packed x and y coordinates</returns>
        inline unsigned long PackShorts(unsigned short x, unsigned short y) noexcept
        {
            return (static_cast<unsigned long>(y) << 16 | static_cast<unsigned long>(x));
        }

        // # action functions

        /// <summary>
        /// Build a building at the given position.
        /// </summary>
        /// <param name="building">S4Building to build</param>
        /// <param name="x">X position</param>
        /// <param name="y">Y position</param>
        void BuildBuilding(S4Building building, unsigned short x, unsigned short y) noexcept
        {
            SendNetEventWrapper(S4EventId::BuildingBuild, static_cast<unsigned long>(building), PackShorts(x, y));
        }

        /// <summary>
        /// Move settlers the given position.
        /// </summary>
        /// <param name="settlers">Settlers to move (MAKE SURE THIS ARRAY IS ALLOCATED ON THE HEAP, THE GAME WILL FREE IT!)</param>
        /// <param name="settlersCount">Amount of settlers to move</param>
        /// <param name="type">Movement type</param>
        /// <param name="x">X position</param>
        /// <param name="y">Y position</param>
        void MoveSettlers(unsigned short* settlers, size_t settlersCount, unsigned short x, unsigned short y, S4MovementType type = S4MovementType::Normal) noexcept
        {
            SendNetEventWrapper(S4EventId::SettlersMove, PackShorts(x, y), static_cast<unsigned short>(type), settlers, settlersCount * sizeof(unsigned short));
        }

        void RecruitCivil(S4Settler type, unsigned long amount, unsigned short ecoSector = 0) noexcept
        {
            SendNetEventWrapper(S4EventId::RecruitCivil, PackShorts(ecoSector, static_cast<unsigned short>(type)), amount);
        }

        void RecruitSpecial(S4Settler type, unsigned long amount, unsigned short ecoSector = 0) noexcept
        {
            SendNetEventWrapper(S4EventId::RecruitSpecial, PackShorts(ecoSector, static_cast<unsigned short>(type)), amount);
        }

        void RecruitMilitary(S4Settler type, unsigned long amount, unsigned short building) noexcept
        {
            SendNetEventWrapper(S4EventId::RecruitMilitary, PackShorts(building, static_cast<unsigned short>(type)), amount);
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

        SearchFnWrapper(Game::SendNetEvent, S4MainModule, Patterns::SendNetEvent);
        SearchFnWrapper(Game::SendLocalEvent, S4MainModule, Patterns::SendLocalEvent);
        SearchFnWrapper(Game::GenerateBuildPoints, S4MainModule, Patterns::GenerateBuildPoints);
        SearchFnWrapper(Game::_ShowTextMsg, S4MainModule, Patterns::ShowTextMsg);
        SearchFnWrapper(Game::_WorldToScreen, S4MainModule, Patterns::WorldToScreen);

        return true;
    }
}