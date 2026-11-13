// 1.13c D2Game+0x73570 -- player-NPC style state machine (fastcall ECX = game, EDX = unit; ret 8, two unused words). rec = 0xA4C0(unit); st = rec[0x50] (signed; <= 4 returns), ctx = rec[0x5C].
// The 0xD event 0x8EA80(game, unit; 0xD, game[0xA8] + d, 0, 0, 0) is posted with d: st 5 becomes 6 (+1), 7 becomes 8 (+1), 9 becomes 10 (+1), 0xC becomes 0xD (+4), 0xE becomes 0xF (+1),
// 8 becomes 0xB when ctx[0x14] != 0 (+0x7D) else 3 with no post; 0xF becomes 0x10 (+5, unit[0xC8] &= ~0x400000 first); 0xB: ctx[0x14] >= now posts +0x19, else state 3,
// unit[0xC8] &= ~0xC00000, ctx[0x14] = 0 and 0x8B340(EAX = (v & ~0xFF) | 0xF; v) with v = unit && !unit[0] ? 0xA4C0(unit)[0x9C] : 0. 6 becomes 3, 0xA/0xD become 0 (each then clears
// unit[0xC8] &= ~0xC00000). 0x10: when unit[0x6C] != 0 and unit[0x68] == 0 the owner p = 0xE03A0(ECX game, EDX unit[0x64]) is looked up; if both units are in state 1 or 2, or neither
// is 0x9B570(ECX unit), 0x719A0(EAX game; p, rec, rec(p), 0xC) runs and both rec[0x58] = now; otherwise state stays 0x10 and +5 is posted. Other states return. `now` is
// GetTickCount (the FN descriptor pins it). Void.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
static void post73570(NE& e, uint32_t game, uint32_t unit, uint32_t d)
{
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xD, e.rd32(game + 0xA8) + d, 0, 0, 0);
}
static void clr73570(NE& e, uint32_t unit, uint32_t m)
{
    if (unit) e.wr32(unit + 0xC8, e.rd32(unit + 0xC8) & m);
}
void f73570(NE& e, uint32_t game, uint32_t unit, uint32_t now)
{
    const uint32_t rec = e.imp(0xA4C0, unit);
    const int32_t st = (int32_t)e.rd32(rec + 0x50);
    if (st < 5) return;
    if (st == 7) { e.wr32(rec + 0x50, 8); post73570(e, game, unit, 1); return; }
    if (st == 0xE) { e.wr32(rec + 0x50, 0xF); post73570(e, game, unit, 1); return; }
    const uint32_t ctx = e.rd32(rec + 0x5C);
    if (st == 8) {
        clr73570(e, unit, 0xFFBFFFFFu);
        if (e.rd32(ctx + 0x14) == 0) { e.wr32(rec + 0x50, 3); return; }
        e.wr32(rec + 0x50, 0xB);
        post73570(e, game, unit, 0x7D);
        return;
    }
    if (st == 0xF) {
        clr73570(e, unit, 0xFFBFFFFFu);
        e.wr32(rec + 0x50, 0x10);
        post73570(e, game, unit, 5);
        return;
    }
    if (st == 0x10) {
        const uint32_t flag = e.rd8(unit + 0x6C), w68 = e.rd32(unit + 0x68), w64 = e.rd32(unit + 0x64);
        if (flag && w68 == 0) {
            const uint32_t p = e.h(0xE03A0, 0, game, w64);
            if (p) {
                const uint32_t r1 = e.imp(0xA4C0, unit);
                const uint32_t r2 = e.imp(0xA4C0, p);
                const uint32_t s1 = e.rd32(r1 + 0x50), s2 = e.rd32(r2 + 0x50);
                bool go = (s1 == 2 || s1 == 1) && (s2 == 2 || s2 == 1);
                if (!go) go = e.h(0x9B570, 0, unit) == 0 && e.h(0x9B570, 0, p) == 0;
                if (go) {
                    e.h(0x719A0, game, 0, 0, 0, 0, p, r1, r2, 0xC);
                    e.wr32(r2 + 0x58, now);
                    e.wr32(r1 + 0x58, now);
                    return;
                }
            }
        }
        post73570(e, game, unit, 5);
        return;
    }
    switch (st) {
    case 5: e.wr32(rec + 0x50, 6); post73570(e, game, unit, 1); return;
    case 6: e.wr32(rec + 0x50, 3); clr73570(e, unit, 0xFF3FFFFFu); return;
    case 9: e.wr32(rec + 0x50, 10); post73570(e, game, unit, 1); return;
    case 10: case 13: e.wr32(rec + 0x50, 0); clr73570(e, unit, 0xFF3FFFFFu); return;
    case 0xB:
        if (e.rd32(ctx + 0x14) >= now) { post73570(e, game, unit, 0x19); return; }
        e.wr32(rec + 0x50, 3);
        clr73570(e, unit, 0xFF3FFFFFu);
        e.wr32(ctx + 0x14, 0);
        {
            uint32_t v = 0;
            if (unit && e.rd32(unit) == 0) v = e.rd32(e.imp(0xA4C0, unit) + 0x9C);
            e.h(0x8B340, (v & ~0xFFu) | 0xF, 0, 0, 0, 0, v);
        }
        return;
    case 0xC: e.wr32(rec + 0x50, 0xD); post73570(e, game, unit, 4); return;
    default: return;
    }
}
}
