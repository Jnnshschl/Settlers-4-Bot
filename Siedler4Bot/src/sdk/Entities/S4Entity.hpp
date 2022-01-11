#pragma once

#include "S4EntityType.hpp"
#include "S4EntityBaseType.hpp"

class S4Entity
{
public:
    unsigned char pad0[4];
    unsigned int Id;
    unsigned short Id2;
    S4EntityType Type;
    unsigned char pad1[1];
    unsigned short ObjectId;
    unsigned char pad2[6];
    S4EntityBaseType BaseType;
    unsigned char SelectionFlags;
    unsigned char pad3[2];
    unsigned short X;
    unsigned short Y;
    unsigned char pad4[4];
    unsigned char Player;
    unsigned char Nation;
    unsigned char Health;

    constexpr unsigned int GetPlayerId() { return (Player & (~0xF)) / 16; }
};