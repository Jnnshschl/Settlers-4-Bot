#pragma once

enum class S4MovementType : unsigned long
{
    Normal,
    Patrol,
    Accumulate,
    Watch = 4,
    Stop = 8,
};
