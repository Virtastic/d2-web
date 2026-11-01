// src/core/d2game/d2game_edc0.cpp -- 1.13c D2Game+0x0EDC0.
// EAX = game, EBX = unit; plain ret, returns 0/1. Records the unit's skill number in the game's 0x1000-bit seen-set
// (game+0x1b24) unless the skill record's flag byte matches gdwBitMasks[1].
#include <cstdint>
#include <cstring>
#include "d2game_seam.h"
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
// 1.13c D2Game+0x0EDC0. `tables` = *sgptDataTables (+0xc24 skills table, +0xc28 count; records 0x14c bytes); `masks` = gdwBitMasks (32 dwords).
extern "C" uint32_t D2Game_sub_EDC0(const D2GameSeam& S, const void* tables, const uint32_t* masks, void* game, void* unit)
{
    const uint32_t g = (uint32_t)(uintptr_t)game, u = (uint32_t)(uintptr_t)unit;
    uint32_t rec = S(0xA53E, 0, 0, 0, 0, 0, unit ? rd32(unit, 4) : 0xFFFFFFFFu);
    if (!rec) return 0;
    int32_t sk = (int32_t)S(0xA874, 0, 0, 0, 0, 0, u);
    if (sk < 0 || sk >= (int32_t)rd32(tables, 0xc28)) return 0;
    const uint8_t* skills = (const uint8_t*)(uintptr_t)rd32(tables, 0xc24);
    const uint32_t r2a = (uint32_t)(uintptr_t)skills + (uint32_t)sk * 0x14c;
    if (!r2a) return 0;
    const uint8_t* r2 = (const uint8_t*)(uintptr_t)r2a;
    if (!((uint8_t)masks[1] & r2[0x2c])) {
        if (!*((const uint8_t*)(uintptr_t)rec + 0x12a) && S(0xE800, u) != 0) return 0;
        if (sk > 0x1000) return 0;
        uint32_t* w = (uint32_t*)((uint8_t*)game + 0x1b24 + (sk >> 5) * 4);
        *w |= masks[sk & 0x1f];
    }
    return 1;
}
