// 1.13c D2Game+0x6A4E0 -- unit event (fastcall ECX = game, EDX = unit): 0xA478(unit) is called and discarded; the game's seed pair (game +0x10F0: lo, hi) steps as lo' = lo * 0x6AC690C5 + hi (64 bit,
// hi' = the high half) and n = (lo' & 1) + 1. 0xB510(EAX = &{game, unit}) gives an index that must lie in [0, dt +0xA80); then 0x6A050(ESI = &{game, unit}, 8) runs n times. Void.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
void f6a4e0(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    e.imp(0xA478, unit);
    const uint32_t seed = e.rd32(game + 0x10f0);
    const uint64_t p = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)p);
    e.wr32(seed + 4, (uint32_t)(p >> 32));
    const int32_t n = (int32_t)(((uint32_t)p & 1u) + 1u);
    uint32_t pair[2] = { game, unit };
    const int32_t idx = (int32_t)e.h(0xB510, e.stk(pair));
    e.blk(0xB510, pair, 2);
    if (idx < 0 || idx >= (int32_t)e.rd32(dt + 0xa80)) return;
    for (int32_t i = 0; i < n; i++) {
        e.h(0x6A050, 0, 0, 0, 0, e.stk(pair), 8);
        e.blk(0x6A050, pair, 2);
    }
}
}
