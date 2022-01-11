#pragma once

#include "Entities/S4Entity.hpp"

enum class S4EventId : unsigned long
{
    None = 0,

    MouseMovement = 5,
    MouseMovementWindow = 6,
    MouseLeftDown = 7,
    MouseLeftUp = 8,
    MouseRightDown = 9,
    MouseRightUp = 10,

    GameTick = 21, // not sure
    WindowFocusChanged = 22,

    EnterBuildMode = 437,

    SecondlyTick = 604, // ticks every 1s as local event

    SettlersMove = 5014,

    BuildingBuild = 5025,

    RecruitCivil = 5027,
    RecruitSpecial = 5039,
    RecruitMilitary = 5047,
};

struct S4Event
{
    void* VTable{ nullptr };
    S4EventId Id{ S4EventId::None };
    unsigned long Params[2]{ 0, 0 };
    unsigned long Tick{ 0 };
    unsigned char Flags{ 0 };
    char Pad0[3]{ 0, 0, 0 };
    unsigned short* Settlers{ nullptr };
    unsigned short SettlersSize{ 0 }; // divide by sizeof(unsigned short) as the size in bytes is specified
    unsigned char Player{ 0 };
    char Pad1{ 0 };

    __forceinline unsigned short* P0UShort() { return reinterpret_cast<unsigned short*>(Params); }
    __forceinline unsigned short* P1UShort() { return reinterpret_cast<unsigned short*>(Params + 1); }
};
