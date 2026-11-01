// src/core/d2game/d2game_e150.cpp -- 1.13c D2Game+0x0E150.
// ECX = unit, EAX = inventory override (0 = use the unit's), EDI = item; plain ret. 1 when the item is the cursor item
// or sits in the inventory's item chain, else 0. A unit with no inventory is a fatal assert (line 0xAB4).
#include <cstdint>
#include <cstring>
#include "d2game_seam.h"
// 1.13c D2Game+0x0E150
extern "C" uint32_t D2Game_sub_E150(const D2GameSeam& S, uint32_t unit, uint32_t inv, uint32_t item)
{
    uint32_t pInv; std::memcpy(&pInv, (const uint8_t*)(uintptr_t)unit + 0x60, 4);
    if (!pInv) { S(0xA2DA, 0, 0, 0, 0, 0, 0xAB4); S(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u); S(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu); return 0; }
    if (inv) pInv = inv;
    if (item == S(0xA6A6, 0, 0, 0, 0, 0, pInv)) return 1;
    uint32_t it = S(0xA3DC, 0, 0, 0, 0, 0, pInv);
    while (it) {
        if (item == S(0xA46C, 0, 0, 0, 0, 0, it)) return 1;
        it = S(0xA4CC, 0, 0, 0, 0, 0, it);
    }
    return 0;
}
