// src/core/d2game/d2game_ffc0.cpp -- 1.13c D2Game+0x0FFC0.
// ESI = seed*, EDI = level, stack (a0, a1, a2, a3), ret 0x10. Picks one of four paths by a roll on the seed.
#include <cstdint>
#include <cstring>
#include "d2game_seam.h"
// 1.13c D2Game+0x0FFC0
extern "C" int32_t D2Game_sub_FFC0(const D2GameSeam& S, int32_t* gl /* [DAT_6fd39fa0 cache, DAT_6fd39fa4 flags] */, uint32_t seedp, int32_t level, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    if (!(gl[1] & 1)) { gl[1] |= 1; gl[0] = (int32_t)S(0xAE74, 0, 0, 0, 0, 0, 0x20646c67u); }
    const int32_t cache = gl[0];
    auto fatal = [&](uint32_t line) -> int32_t {
        const uint32_t m = S(0xA2DA, 0, 0, 0, 0, 0, line);
        S(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, m);
        S(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return 0;
    };
    if (cache == -1) return fatal(0x17c);
    if (cache == 0) return fatal(0x17f);
    if (level > 0x41) return fatal(0x180);
    const int32_t half = level >> 1;
    int32_t a = half + 10; if (level & 1) a++;
    const int32_t b = half + 5 + (0x41 - level);
    uint32_t lo, hi; std::memcpy(&lo, (void*)(uintptr_t)seedp, 4); std::memcpy(&hi, (void*)(uintptr_t)(seedp + 4), 4);
    const uint64_t n = (uint64_t)lo * 0x6ac690c5u + hi;
    std::memcpy((void*)(uintptr_t)seedp, &n, 8);
    const int32_t r = (int32_t)((uint32_t)n % 100u);
    if (r < 0x41 - level) return cache;
    if (r < b) return (int32_t)S(0xFA30, 0, a1, 0, 0, 0, a0, seedp, (uint32_t)level, a2);
    if (r < b + a) return (int32_t)S(0xF8F0, 0, a1, 0, 0, 0, a0, seedp, (uint32_t)level, a2);
    if (r < 100) return (int32_t)S(0xF780, 0, 0, 0, 0, 0, a0, seedp, (uint32_t)level, a1);   // a2, a3 beyond the four words S carries
    return fatal(0x1ba);
}
