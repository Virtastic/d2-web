// 1.13c D2Game+0xC54A0 -- quest-event callback, ECX = game, EDX = unit, one ignored stack word (ret 4), returns 0. Two steps on the unit's quest-flag buffer (0xA4C0, byte game+0x6D): state 9 flag 5 set walks the game's list holder (+0x10F4) for the kind 8 node and, when 0xE660(unit, "ass ") answers 0, bumps the dword at [[node+0x18]+0xC]; state 9 flag 0xD set marks the unit (word +0x6E = 0x32, +0x70 = itself, 0xA4DE, flag 0x400 in +0xC4) and is fatal for a null unit.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc54a0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d)), 9, 5)) {
        const uint32_t holder = e.rd32(game + 0x10f4);
        if (holder)
            for (uint32_t n = e.rd32(holder); n; n = e.rd32(n + 0xf4))
                if (e.rd32(n) == 8) {
                    if (!e.h(0xE660, unit, 0, 0, 0, 0, 0x20737361)) { const uint32_t p = e.rd32(n + 0x18); e.wr32(p + 0xc, e.rd32(p + 0xc) + 1); }
                    break;
                }
    }
    pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d)), 9, 0xd)) {
        if (!unit) {   // fatal path, not vectored
            const uint32_t msg = e.h(0xA2DA, 0, 0, 0, 0, 0, 0x558);
            e.h(0xA2C2, 0, 0, 0, 0, 0, 0x6fd19ff9, msg);
            e.h(0x2A3C, 0, 0, 0, 0, 0, 0xffffffffu);
        }
        e.wr8(unit + 0x6e, 0x32); e.wr8(unit + 0x6f, 0);
        e.wr32(unit + 0x70, unit);
        e.h(0xA4DE, 0, 0, 0, 0, 0, unit);
        e.wr32(unit + 0xc4, e.rd32(unit + 0xc4) | 0x400);
    }
    return 0;
}
}
