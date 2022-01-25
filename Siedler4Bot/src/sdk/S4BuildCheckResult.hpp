#pragma once

enum class S4BuildCheckResult : int
{
    InvalidOwner = -1,
    Blocked = 0,
    Okay = 1,

    // TODO: find the meaning of this returns, maybe other blocked states or size/heigth problems?
    // Unk0 = 2,
    // Unk1 = 3,
    // Unk2 = 4,
    // Unk3 = 5,
    // Unk4 = 6,
};
