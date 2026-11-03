// 1.13c D2Game+0x5A820 -- ACT5Q6_RollEligibleRow, ECX = pointer whose first dword is the game. Finds the game's type 0x24 list entry,
// and while its state (+0x18) has no row (+0xA0 == 0) rolls up to ten rows with the game's RNG (+0x10F4 holds the seed pair at +0x18/+0x1C)
// out of the 0xA80-count table at +0xA78 (stride 0x1A8) until one has a bit of gdwBitMasks[6] in byte +0xE; failing that it takes row 0.
#include <cstring>
#include "d2game_5a820.h"
namespace {
uint32_t rd(uint32_t b, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(b + o), 4); return v; }
void wr(uint32_t b, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(b + o), &v, 4); }
uint8_t* bp(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
}  // namespace
void d2game_port_5a820(uint32_t p1, uint32_t dt, uint32_t bitMasks, const D2Game5A820Env* env)
{
    const uint32_t game = rd(p1, 0);
    uint32_t q = 0;
    if (const uint32_t list = rd(game, 0x10f4))
        for (uint32_t p = rd(list, 0); p; p = rd(p, 0xf4)) if (rd(p, 0) == 0x24) { q = p; break; }
    const uint32_t st = rd(q, 0x18);
    if (*bp(st + 0xa0)) return;
    const uint32_t n = rd(dt, 0xa80);
    int tries = 0;
    do {
        if (tries >= 10) break;
        const uint32_t rng = rd(rd(q, 4), 0x10f4);
        if (!rng) { env->fatal(); return; }
        uint32_t v;
        if ((int32_t)n < 2) v = 1;
        else {
            const uint64_t m = (uint64_t)rd(rng, 0x18) * 0x6ac690c5ull + rd(rng, 0x1c);
            wr(rng, 0x18, (uint32_t)m); wr(rng, 0x1c, (uint32_t)(m >> 32));
            v = (((n - 1) & (n - 2)) == 0 ? ((n - 2) & (uint32_t)m) : (uint32_t)m % (n - 1)) + 1;
        }
        ++tries;
        wr(st, 0xa4, v);
        if ((int32_t)v >= 0 && (int32_t)v < (int32_t)rd(dt, 0xa80)) {
            const uint32_t rec = v * 0x1a8 + rd(dt, 0xa78);
            if (rec && (*bp(rec + 0xe) & rd(bitMasks, 0x18))) *bp(st + 0xa0) = 1;
        }
    } while (!*bp(st + 0xa0));
    if (!*bp(st + 0xa0)) { *bp(st + 0xa0) = 1; wr(st, 0xa4, 0); }
    env->dfcc0(game);
}
