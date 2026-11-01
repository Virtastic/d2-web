// 1.13c D2Common+0x444F0
// Marks a room 0x800000 when one of its neighbours lies in a town level: EDX = room. The room's level id (level +0x1D0, level at room +0x58)
// is tested first with DRLG_IsTownLevel (0x2D320; a town id -> nothing happens); otherwise the neighbour list (room +8, count at +0x2C) is walked and the first
// neighbour whose level is a town sets the flag at room +0x28. Links are 32-bit retail tokens, `resolve` turns one into an address.
#include <cstdint>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"

extern "C" uint32_t D2Common_444F0(uint8_t* pRoom, uint8_t* (*resolve)(uint32_t))
{
    auto rd = [](const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; };
    auto levelId = [&](const uint8_t* room) { return static_cast<int32_t>(rd(resolve(rd(room, 0x58)), 0x1D0)); };
    if (uint32_t t = static_cast<uint32_t>(DRLG_IsTownLevel(levelId(pRoom)))) return t;   // EAX at the return is the town test, or the count below
    if (static_cast<int32_t>(rd(pRoom, 0x2C)) <= 0) return rd(pRoom, 0x2C);
    const uint32_t list = rd(pRoom, 8);
    for (int32_t i = 0;;) {
        const uint8_t* neighbour = resolve(rd(resolve(list + 4 * i), 0));
        if (uint32_t t = static_cast<uint32_t>(DRLG_IsTownLevel(levelId(neighbour)))) {
            uint32_t flags = rd(pRoom, 0x28) | 0x800000;
            std::memcpy(pRoom + 0x28, &flags, 4);
            return t;
        }
        ++i;
        if (i >= static_cast<int32_t>(rd(pRoom, 0x2C))) return rd(pRoom, 0x2C);
    }
}
