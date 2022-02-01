#pragma once

#include "Win32.hpp"

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

/// <summary>
/// Helper function to check whether coordinates are on screen or not.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
inline bool IsOnScreen(int x, int y, int width, int height) noexcept
{
    return x >= 0 && y >= 0 && x <= width && y <= height;
}

/// <summary>
/// This helper is used to trigger stuff only once when a key 
/// gets pressed instead of spamming it.
/// </summary>
/// <param name="vKey">Key to observe</param>
/// <param name="switchVar">Static boolean</param>
/// <returns>True when the key got pressed, but only once</returns>
inline bool OnKeyPressedOnce(unsigned char vKey, bool& sVar)
{
    if (KEY_PRESSED(vKey))
    {
        if (!sVar)
        {
            sVar = true;
            return sVar;
        }
    }
    else if (sVar)
    {
        sVar = false;
    }

    return false;
}
