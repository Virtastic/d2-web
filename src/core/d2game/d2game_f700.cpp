// src/core/d2game/d2game_f700.cpp -- 1.13c D2Game+0x0F700.
// Stack arg = unit; ret 4. Rolls a value for the unit's record range [rec+0xcc, rec+0xd0] with 0x1480 (inlined: the unit+0x20 seed,
// range size, low bound) and stores it as stat 0x1f. A missing record or a roll above the range logs and exits
// the process in retail (0xA2DA/0xA2C2/_exit(-1)); the port returns there.
#include <cstdint>
#include <cstring>
#include "d2game_seam.h"
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
// 1.13c D2Game+0x0F700
extern "C" void D2Game_sub_F700(const D2GameSeam& S, void* unit)
{
    const uint32_t u = (uint32_t)(uintptr_t)unit;
    uint32_t rec = S(0xA53E, 0, 0, 0, 0, 0, unit ? rd32(unit, 4) : 0xFFFFFFFFu);
    if (!rec) return;
    const void* rp = (const void*)(uintptr_t)rec;
    uint32_t hi = rd32(rp, 0xd0), lo = rd32(rp, 0xcc);
    // 1.13c D2Game+0x1480 (inlined: ECX = seed qword, ESI = range, EDI = base): advance the seed, roll within [base, base+range).
    int32_t range = (int32_t)(hi - lo + 1);
    uint32_t r = lo;
    if (range >= 1) {
        uint64_t sd; std::memcpy(&sd, (uint8_t*)unit + 0x20, 8);
        sd = (uint64_t)(uint32_t)sd * 0x6ac690c5ull + (uint64_t)(uint32_t)(sd >> 32);
        std::memcpy((uint8_t*)unit + 0x20, &sd, 8);
        r = (((uint32_t)range & ((uint32_t)range - 1)) != 0 ? (uint32_t)sd % (uint32_t)range : (uint32_t)range - 1 & (uint32_t)sd) + lo;
    }
    if (r > hi) return;
    S(0xA472, 0, 0, 0, 0, 0, u, 0x1f, r, 0);
}
