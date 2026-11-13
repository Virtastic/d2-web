// 1.13c D2Game+0x9CC00 -- ECX game, EDX unit, one stack word (ret 4): ctl, a control block ([0] a record whose +0x14 is a flag, [+8] a word handed on, [+0x14] a count). No result.
// Tail (unit of type 1 whose pData (+0x14) word +0x54 is 3 or 0x13, or count < 4): 0x9D230(ECX game, EDX unit, ctl[0], 0) then 0xF22C0(EAX game, ESI unit, 10).
// Else count > 10 and the flag clear: flag = 1, 0xF2870(AL 0xC, ECX ctl[8], game, unit). Else one seed roll (x * 0x6AC690C5 + hi, stored back); low byte < 0x4D: 0xF1B10(AL 4, ECX new hi, game, unit);
// else a second roll, low byte < 0x1A clears the flag, then 0xF22C0(EAX game, ESI unit, 10).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void f9cc00(GE& e, uint32_t game, uint32_t unit, uint32_t ctl)
{
    const int32_t cnt = (int32_t)e.rd32(ctl + 0x14);
    bool tail = false;
    if (unit && e.rd32(unit) == 1) {
        const uint32_t pd = e.rd32(unit + 0x14);
        if (pd) { const uint32_t t = e.rd32(pd + 0x54); if (t == 3 || t == 0x13) tail = true; }
    }
    if (!tail && cnt < 4) tail = true;
    const uint32_t c0 = e.rd32(ctl);
    if (tail) {
        e.h(0x9D230, 0, game, unit, 0, 0, c0, 0);
        e.h(0xF22C0, game, 0, 0, 0, unit, 10);
        return;
    }
    if (cnt > 10 && e.rd32(c0 + 0x14) == 0) {
        e.wr32(c0 + 0x14, 1);
        e.h(0xF2870, 0xC, e.rd32(ctl + 8), 0, 0, 0, game, unit);
        return;
    }
    const uint64_t s = (uint64_t)e.rd32(unit + 0x20) * 0x6ac690c5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(s >> 32));
    e.wr32(unit + 0x20, (uint32_t)s);
    if (((uint32_t)s & 0xff) < 0x4d) {
        e.h(0xF1B10, 4, (uint32_t)(s >> 32), 0, 0, 0, game, unit);
        return;
    }
    const uint64_t s2 = (uint64_t)(uint32_t)s * 0x6ac690c5u + (uint32_t)(s >> 32);
    e.wr32(unit + 0x20, (uint32_t)s2);
    e.wr32(unit + 0x24, (uint32_t)(s2 >> 32));
    if (((uint32_t)s2 & 0xff) < 0x1a) e.wr32(c0 + 0x14, 0);
    e.h(0xF22C0, game, 0, 0, 0, unit, 10);
}
}
