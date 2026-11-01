// 1.13c D2Game+0x471E0 -- pick a random spot in a room where a monster group fits (stdcall: game, room, coord list, monster id, group, &x, &y; ret 0x1C).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h).
// 0x46C30(EAX = list, EDI/ESI/EBX = &baseX/&baseY/&rangeY; room, &rangeX) fills the four consecutive locals rangeX, baseX, rangeY, baseY.
// want = list +0x28 (with a list). The room's seed (room +0x6C: lo, hi) steps as lo * 0x6AC690C5 + hi (64-bit, stored back); a roll over
// r <= 0 is 0 without a step, else the new lo & (r - 1) for a power of two, else lo % r. Up to 20 tries: x = roll(rangeX) + baseX,
// y = roll(rangeY) + baseY; each group entry j (group [0] = count, entries of three words from group +0xC: dx, dy) must pass: with a list,
// 0xAD66(room, dx + x, dy) == want; then 0xEEF10(ECX = the 10-word spawn request {game, room, list or 0, monster id, 1, 0, dx + x, dy + y,
// -1, short 1}) != 0. All pass: *px = x, *py = y, 1. Twenty failures: 0.
#include "../fn/d2game_gp.h"
namespace d2g {
static uint32_t roll471e0(GE& e, uint32_t seed, int32_t r)
{
    if (r < 1) return 0;
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5u + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)v); e.wr32(seed + 4, (uint32_t)(v >> 32));
    const uint32_t ur = (uint32_t)r;
    return (ur & (ur - 1)) == 0 ? (uint32_t)v & (ur - 1) : (uint32_t)v % ur;
}
uint32_t f471e0(GE& e, uint32_t game, uint32_t room, uint32_t list, uint32_t mon, uint32_t grp, uint32_t px, uint32_t py)
{
    int32_t rg[4] = {};   // rangeX, baseX, rangeY, baseY
    e.h(0x46C30, list, 0, 0, 0, 0, room, e.stk(rg));
    const uint32_t want = list ? e.rd32(list + 0x28) : 0;
    const uint32_t seed = room ? room + 0x6C : 0;
    const int32_t cnt = (int32_t)e.rd32(grp);
    for (int t = 0; t < 20; t++) {
        const uint32_t x = roll471e0(e, seed, rg[0]) + (uint32_t)rg[1];
        const uint32_t y = roll471e0(e, seed, rg[2]) + (uint32_t)rg[3];
        int32_t j = 0;
        for (; j < cnt; j++) {
            const uint32_t ent = grp + 0xC + 0xC * (uint32_t)j, dx = e.rd32(ent), dy = e.rd32(ent + 4);
            if (list && e.h(0xAD66, 0, 0, 0, 0, 0, room, dx + x, dy) != want) break;
            uint32_t q[10] = { game, room, list, mon, 1, 0, dx + x, dy + y, ~0u, 1 };
            const uint32_t ok = e.h(0xEEF10, 0, e.stk(q));
            e.blk(0xEEF10, q, 9);
            if (!ok) break;
        }
        if (j >= cnt) { e.wr32(px, x); e.wr32(py, y); return 1; }
    }
    return 0;
}
}
