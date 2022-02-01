#pragma once

#include <cmath>

inline unsigned short CalcDistance(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2) noexcept
{
    return static_cast<unsigned short>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}
