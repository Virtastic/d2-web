// 1.13c D2Game+0x266B0: ECX = game, EDX = unit; two ignored stack words. Unique monster burst. The unit position (types 2, 4..5 static; else #A454 / #A3B8
// on the path) is the spawn point of missile 0x75 (#A28F0: ECX = game, EDX = 0x75, EBX = unit; unit, 0, 1, 0, 0, x, y, 1); no missile ends it. With
// f = 1 when game[0x6A] or game[0x74] is set, #AD7E(class, f, game[0x6D], #A42A(unit, 0xC, 0), 1, &blk) fills a 14-dword block whose dword 1 goes through
// #14D0(EAX = #A7BA(game[0x6D])[0x3C], ECX = 100, EDX = it) to s. s is cut by the difficulty byte (0: -1/4, 1: -1/3, 2: 1/8, else kept); d = s * 60 / 100
// (truncating; above 0x100000 either 64-bit or (s / 100) * 60). The unit seed rolls 0..s-d-1 (#11D0, ESI = s - d); dmg = (roll + d) << 6 sits in dwords 2 and 4
// of a 28-dword block sent with the missile to #DE380 (game, missile, x, y, byte + 4, &block, 0, 0, 0, 0x581; EBX = unit, ESI = s - d).
#include "d2game_f1pos.h"
static inline int32_t f1Div100b(int32_t v)
{
    const int32_t hi = (int32_t)(((int64_t)0x51eb851f * v) >> 32), q = hi >> 5;
    return q + (int32_t)((uint32_t)q >> 31);
}
static inline uint32_t f1Roll(uint32_t seed, int32_t limit)
{
    if (limit <= 0) return 0;
    const uint64_t n = (uint64_t)d2rd32(seed) * 0x6AC690C5u + d2rd32(seed + 4);
    d2wr32(seed, (uint32_t)n); d2wr32(seed + 4, (uint32_t)(n >> 32));
    return (uint32_t)n % (uint32_t)limit;
}
extern "C" void d2g_266b0(uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t x = f1PosX(unit, e), y = f1PosY(unit, e);
    const uint32_t missile = e->call(0xA28F0, 0, game, 0x75, unit, 0, unit, 0, 1, 0, 0, x, y, 1);
    if (!missile) return;
    const uint32_t f = (d2rd8(game + 0x6A) || d2rd32(game + 0x74)) ? 1 : 0;
    uint32_t blk1[14] = {};
    const uint32_t gb = d2rd8(game + 0x6D), cls = d2rd32(unit + 4);
    const uint32_t level = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xc, 0);
    e->call(0xAD7E, 0, 0, 0, 0, 0, cls, f, gb, level, 1, (uint32_t)(uintptr_t)blk1);
    { const uint32_t w[2] = { 1, (uint32_t)(uintptr_t)blk1 }; e->x(0xAD7E, w, 2); }
    const uint32_t rec = e->call(0xA7BA, 0, 0, 0, 0, 0, gb);
    if (!rec) return;
    int32_t s = (int32_t)e->call(0x14D0, d2rd32(rec + 0x3C), 100, blk1[1]);
    if (d2rd8(game + 0x6D) == 0) {
        s += -((s + ((s >> 31) & 3)) >> 2);
    } else if (d2rd8(game + 0x6D) == 1) {
        const int32_t hi = (int32_t)(((int64_t)0x55555555 * s) >> 32);
        int32_t t = (hi - s) >> 1; t += (int32_t)((uint32_t)t >> 31);
        s += t;
    } else if (d2rd8(game + 0x6D) == 2) {
        s = (s + ((s >> 31) & 7)) >> 3;
    }
    int32_t d;
    if (s <= 0x100000) d = f1Div100b((int32_t)((uint32_t)s * 0x3c));
    else if ((int32_t)(s & ~0xf) < 0x640) d = (int32_t)((int64_t)s * 60 / 100);
    else d = f1Div100b(s) * 0x3c;
    const int32_t lim = s - d;
    const uint32_t roll = f1Roll(unit + 0x20, lim);
    const uint32_t dmg = (roll + (uint32_t)d) << 6;
    uint32_t blk2[28] = {};
    blk2[2] = dmg; blk2[4] = dmg;
    const uint32_t w[10] = { game, missile, x, y, gb + 4, (uint32_t)(uintptr_t)blk2, 0, 0, 0, 0x581 };
    e->call(0xDE380, 0, 0, 0, unit, (uint32_t)lim, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    e->x(0xDE380, w + 4, 6);
    e->blk(0xDE380, blk2, 28);
}
