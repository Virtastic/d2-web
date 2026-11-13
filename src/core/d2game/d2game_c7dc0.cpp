// 1.13c D2Game+0xC7DC0 -- quest event callback, no result. ECX = quest record, EDX = event {+0 unit, +8, +0xC player, +0x14 kind word, +0x18 code word}.
// pd = 0xA4C0(player); buf = pd[0x10 + 4*unit.byte(+0x6D)]. Only kind 0xF4 acts, by code: 0x14F calls 0x366A0 (EAX 0x20317274, ECX unit, ESI player) then
// 0xA57A(buf, 0xA, 3). 0x150/0x151/0x152 clear rec+0x14 and set quest bit 4/5/6 then 3. 0x153: 0xA586(buf, 0xA, 1), set 3, 0x33880 (EAX = rec+0x1C,
// ESI = player's +0xC or -1), set 0xA, 4, 6, 5, then 0x35910 (EAX = ev+8, ECX = player, one stack word = unit).
#include "../fn/hookenv.h"
namespace d2g {
void fc7dc0(HookEnv& e, uint32_t q, uint32_t ev)
{
    const uint32_t unit = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t kind = e.rd32(ev + 0x14) & 0xFFFF, code = e.rd32(ev + 0x18) & 0xFFFF;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(unit + 0x6d));
    if (kind != 0xF4) return;
    auto st = [&](uint32_t b) { e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0xA, b); };
    switch (code) {
    case 0x14F: e.h(0x366A0, 0x20317274, unit, 0, 0, pl); st(3); return;
    case 0x150: case 0x151: case 0x152:
        e.wr8(q + 0x14, 0); st(code - 0x14C); st(3); return;
    case 0x153: {
        e.h(0xA586, 0, 0, 0, 0, 0, buf, 0xA, 1); st(3);
        e.h(0x33880, q + 0x1C, 0, 0, 0, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu);
        st(0xA); st(4); st(6); st(5);
        e.h(0x35910, e.rd32(ev + 8), pl, 0, 0, 0, unit);
        return;
    }
    }
}
}
