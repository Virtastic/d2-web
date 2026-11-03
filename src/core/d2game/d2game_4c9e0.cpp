// 1.13c D2Game+0x4C9E0 chance-gated skill proc: eax=chance, ecx=unit, edx=seed*, stack game, arg2 (ret 8).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c9e0(uint32_t chance, uint32_t unit, uint32_t seed, uint32_t game, uint32_t arg2, const D2Seam* e)
{
    if (!e->call(0xA430,0,0,0,0,0,unit,0x2d)) return 0;
    uint32_t q, r;
    auto roll = [&]() {   // D2 LCG: seed = lo*0x6AC690C5 + hi (64-bit); result lo / 100, lo % 100
        const uint64_t m = (uint64_t)d2rd32(seed + 0x20) * 0x6AC690C5u + d2rd32(seed + 0x24);
        d2wr32(seed + 0x24, (uint32_t)(m >> 32)); d2wr32(seed + 0x20, (uint32_t)m);
        q = (uint32_t)m / 100; r = (uint32_t)m % 100;
    };
    if (e->call(0xA940,0,0,0,0,0,unit)) {
        const uint32_t v = e->call(0xA42A,0,0,0,0,0,unit,0x46,0);
        if ((int32_t)v <= 0) return v;
        roll();
        if ((int32_t)r >= (int32_t)chance) return q;
        return e->call(0xA1790,0,0,0,0,0,game);
    }
    if (!e->call(0xA970,0,0,0,0,0,unit)) return 0;
    roll();
    if ((int32_t)r >= (int32_t)chance) return q;
    const int32_t n = (int32_t)(e->call(0xA42A,0,0,0,0,0,unit,0x48,0) - arg2);
    if (n < 0 || n == 0) return e->call(0xD5740, unit, seed, game);
    e->call(0xA472,0,0,0,0,0,unit,0x48,(uint32_t)n,0);
    uint32_t w = 0;
    if (seed && d2rd32(seed) == 0) { const uint32_t p = e->call(0xA4C0,0,0,0,0,0,seed); w = d2rd32(p + 0x9c); }
    return e->call(0x8B010, unit, 0,0,0,0, w, 1, 0x48, (uint32_t)n);
}
