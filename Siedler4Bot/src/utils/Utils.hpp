#pragma once

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
