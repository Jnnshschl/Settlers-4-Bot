#pragma once

#include "S4GroundType.hpp"

class S4WorldTile
{
public:
    unsigned char Height;
    S4GroundType Type;
    unsigned char Unknown;
    unsigned char Attributes;
};