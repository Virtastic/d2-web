// 1.13c D2Game+0x19880: ECX = game, EDX = unit, one stack word `cd` (ret 4): a pointer to a pointer to a record whose +0x18 is a countdown.
// Think step toward a target: #F30C0 (ESI = unit; game, &scratch) names it. With a target other than the unit, #B0A20 (EBX = unit; target) is the distance
// and the monster AI chain head (unit[0x14][0x30]) is read. A busy player target (type 0 and #37680 == 1) or any chain node whose first dword is
// nonzero ends in #F22C0 (EAX = game, ESI = unit; 0xA). Otherwise the countdown: distance >= 0x18 reloads a non-positive one to 0x3C and decrements it; a nearer
// target with a nonzero countdown decrements, with zero reloads 0x3C (a type 0 target also runs #DFFD0 (ESI = unit; 0x12, target)); all end in
// #F22C0 (0x14). Without a target class 0xFB / 0x196 run #C4AA0 / #B9C00 (EAX = game; unit), #15BB0 (ESI = unit; game), #F21E0 (game, 0, 0)
// and #DFCC0 (EAX = 0, EBX = 0x6FCE4CC0 / 0x6FCD9C60, EDI = game; 0), each step ending in #F22C0 (0x14) when its test fails.
#include "d2game_seam4e.h"
extern "C" void d2g_19880(uint32_t game, uint32_t unit, uint32_t cd, const D2Seam* e)
{
    uint32_t scratch = 0;
    const uint32_t tgt = e->call(0xF30C0, 0, 0, 0, 0, unit, game, (uint32_t)(uintptr_t)&scratch);
    if (tgt && tgt != unit) {
        const int32_t dist = (int32_t)e->call(0xB0A20, 0, 0, 0, unit, 0, tgt);
        uint32_t ai = 0;
        if (unit && d2rd32(unit) == 1) { const uint32_t p = d2rd32(unit + 0x14); if (p) ai = d2rd32(p + 0x30); }
        bool busy = false;
        if (d2rd32(tgt) == 0 && e->call(0x37680, 0, 0, 0, 0, tgt) == 1) busy = true;
        if (!busy) {
            for (uint32_t n = d2rd32(ai); n; n = d2rd32(n + 8)) if (d2rd32(n) != 0) { busy = true; break; }
        }
        if (busy) { e->call(0xF22C0, game, 0, 0, 0, unit, 0xA); return; }
        const uint32_t rec = d2rd32(d2rd32(cd));
        if (dist >= 0x18) {
            if ((int32_t)d2rd32(rec + 0x18) <= 0) d2wr32(rec + 0x18, 0x3C);
            d2wr32(rec + 0x18, d2rd32(rec + 0x18) - 1);
        } else if (d2rd32(rec + 0x18) != 0) {
            d2wr32(rec + 0x18, d2rd32(rec + 0x18) - 1);
        } else {
            d2wr32(rec + 0x18, 0x3C);
            if (d2rd32(tgt) == 0) e->call(0xDFFD0, 0, 0, 0, 0, unit, 0x12, tgt);
            e->call(0xF22C0, game, 0, 0, 0, unit, 0x14);
            return;
        }
        e->call(0xF22C0, game, 0, 0, 0, unit, 0x14);
        return;
    }
    if (unit) {
        const uint32_t cls = d2rd32(unit + 4);
        if (cls == 0xFB || cls == 0x196) {
            if (e->call(cls == 0xFB ? 0xC4AA0 : 0xB9C00, game, 0, 0, 0, 0, unit) &&
                e->call(0x15BB0, 0, 0, 0, 0, unit, game)) {
                e->call(0xF21E0, 0, 0, 0, 0, 0, game, 0, 0);
                e->call(0xDFCC0, 0, 0, 0, cls == 0xFB ? 0x6fce4cc0u : 0x6fcd9c60u, 0, 0);
                e->edi(0xDFCC0, game);
                return;
            }
        }
    }
    e->call(0xF22C0, game, 0, 0, 0, unit, 0x14);
}
