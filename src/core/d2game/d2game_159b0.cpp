// 1.13c D2Game+0x159B0: AI offset helper. Fastcall ECX = game (unused), EDX = unit, one stack word `ai` (ret 4). The target is the
// owner lookup 0xE03A0 over the unit's +0x14, +0x28 record (an assert exit when the chain is missing). The record behind ai[0] receives
// the unit-minus-target x and y (+0x14, +0x18), and when the target is the unit itself a seed roll mod 100 (+0x1C).
#include "d2game_seam4e.h"
static uint32_t d2g_159b0_pos(uint32_t u, uint32_t off, uint32_t getter, const D2Seam* e)
{
    const uint32_t t = d2rd32(u);
    const uint32_t p = d2rd32(u + 0x2C);
    if (t == 2 || (t > 3 && t <= 5)) return d2rd32(p + off);
    return p ? e->call(getter, 0, 0, 0, 0, 0, p) : 0;
}
extern "C" void d2g_159b0(uint32_t game, uint32_t unit, uint32_t ai, const D2Seam* e)
{
    (void)game;
    uint32_t tgt = 0;
    uint32_t a, c, q;
    if (unit && d2rd32(unit) == 1 && (a = d2rd32(unit + 0x14)) && (q = d2rd32(a + 0x28)) && (c = d2rd32(q + 0x28)))
        tgt = e->call(0xE03A0, d2rd32(q + 0x30), c, d2rd32(q + 0x2C));
    else {
        const uint32_t line = e->call(0xA2DA);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0xB2000000u | 0xF9FF9u, line, 0xE17);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return;
    }
    const uint32_t tx = d2g_159b0_pos(tgt, 0xC, 0xA454, e);
    const uint32_t ty = d2g_159b0_pos(tgt, 0x10, 0xA3B8, e);
    const uint32_t ux = d2g_159b0_pos(unit, 0xC, 0xA454, e);
    const uint32_t uy = d2g_159b0_pos(unit, 0x10, 0xA3B8, e);
    const uint32_t rec = d2rd32(ai);
    d2wr32(rec + 0x14, ux - tx);
    d2wr32(rec + 0x18, uy - ty);
    if (tgt == unit) {
        const uint64_t m = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5u + d2rd32(unit + 0x24);
        d2wr32(unit + 0x24, (uint32_t)(m >> 32));
        d2wr32(unit + 0x20, (uint32_t)m);
        d2wr32(rec + 0x1C, (uint32_t)m % 100u);
    }
}
