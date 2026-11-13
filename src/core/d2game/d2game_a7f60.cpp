// 1.13c D2Game+0xA7F60 -- ECX game, EDX unit, one stack word ctl (ret 4). No result. c = ctl[0] (its word +0x14 is a "pending" flag).
// 0xE07A0(EAX unit); t = 0xA598(unit[+0x2C]). t non-null, t != unit and 0xA808(0xA478(t)) zero: flag clear: flag = 1, 0xF3000(ECX t, EDX unit; game, 7), done.
// Flag set: r = 0xA754(unit, t, 0); non-zero: 0xF2220(EAX 4, EDI unit, EBP c; game, t), done. r zero: x = 0xF3220(EAX &r, ECX unit, EDX c; game, &ctl) (it may write r): x and r non-zero: flag = 0,
// 0xF2220(EAX 4, EDI unit; game, x), done; else flag = 1, 0xACD0(unit, t), 0xF3000(ECX t, EDX unit; game, 7).
// Otherwise (no usable t): 0x9D230(ECX game, EDX unit; unit type-1 pData[+0x28] or 0, 0), every chain node (unit[+0xDC], next +0x24) of byte 2 goes to 0x8E6F0(EAX node),
// then 0x8EA80(ECX game, EDX unit; 2, game[+0xA8] + 1, 0, 0, 0).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fa7f60(GE& e, uint32_t game, uint32_t unit, uint32_t ctl)
{
    e.h(0xE07A0, unit);
    const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    bool tail = (t == unit || !t);
    if (!tail) tail = e.h(0xA808, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, t)) != 0;
    if (!tail) {
        const uint32_t c = e.rd32(ctl);
        if (!e.rd32(c + 0x14)) {
            e.wr32(c + 0x14, 1);
            e.h(0xF3000, 0, t, unit, 0, 0, game, 7);
            return;
        }
        volatile uint32_t r = e.h(0xA754, 0, 0, 0, 0, 0, unit, t, 0);
        if (r) {
            e.h(0xF2220, 4, 0, 0, 0, 0, game, t);
            e.y(0xF2220, c, unit);
            return;
        }
        uint32_t slot = ctl;
        const uint32_t x = e.h(0xF3220, (uint32_t)(uintptr_t)&r, unit, c, 0, 0, game, (uint32_t)(uintptr_t)&slot);
        if (x && r) {
            e.wr32(c + 0x14, 0);
            e.h(0xF2220, 4, 0, 0, 0, 0, game, x);
            e.y(0xF2220, c, unit);
            return;
        }
        e.wr32(c + 0x14, 1);
        e.h(0xACD0, 0, 0, 0, 0, 0, unit, t);
        e.h(0xF3000, 0, t, unit, 0, 0, game, 7);
        return;
    }
    uint32_t pv = 0;
    if (e.rd32(unit) == 1) { const uint32_t pd = e.rd32(unit + 0x14); if (pd) pv = e.rd32(pd + 0x28); }
    e.h(0x9D230, 0, game, unit, 0, 0, pv, 0);
    for (uint32_t n = e.rd32(unit + 0xDC); n;) {
        const uint32_t nx = e.rd32(n + 0x24);
        if (e.rd8(n) == 2) e.h(0x8E6F0, n);
        n = nx;
    }
    e.h(0x8EA80, 0, game, unit, 0, 0, 2, e.rd32(game + 0xA8) + 1, 0, 0, 0);
}
}
