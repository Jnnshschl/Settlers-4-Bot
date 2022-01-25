#pragma once

class S4EcoSector
{
public:
	unsigned char PlayerId;
	unsigned char pad0[3];
	unsigned long pad1[4];

	inline unsigned char Player() { return PlayerId & 0xF; }
};
