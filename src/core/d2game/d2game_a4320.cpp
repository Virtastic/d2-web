// 1.13c D2Game+0xA4320 -- ECX game, EDX unit, one stack word ctl (ret 4). No result.
// ctl[0] = c: c[+0x14] = 0, c[+0x18] = 0, c[+0x1C] = 1. Then, only for a type-1 unit whose pData (+0x14) word +0x28 = w is set with w[+0x28] set:
// er = 0xE03A0(EAX w[+0x30], ECX w[+0x28], EDX w[+0x2C]) must be non-null with er[0] == 0. a = 0xA3F4(ECX er, EDX short ctl[+0x1C][+0x80]); non-zero: c[+0x1C] = 0xA400(er, a, 1).
// v = c[+0x1C]. 0xA3D0(ECX unit, EDX 0; -1) zero: 0xA9FA(unit, 0, 1, 0, string, 0xBEE). 0xADF0(unit, 0, -1), 0xAE08(unit, 0, -1). Then for i while i < 0xA418(er[1]):
// node = 0xA442(er[1], i); AL of 0xA3D60(EAX er, ECX node, EDX unit) set: 0xA3D0(ECX unit, EDX node; -1), r = 0xA3D00(ECX v, EDX node; v), 0xA9FA(unit, node, r, 0, string, 0xBFD).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fa4320(GE& e, uint32_t game, uint32_t unit, uint32_t ct)
{
    (void)game;
    const uint32_t c = e.rd32(ct);
    e.wr32(c + 0x14, 0); e.wr32(c + 0x18, 0); e.wr32(c + 0x1C, 1);
    if (!unit || e.rd32(unit) != 1) return;
    const uint32_t pd = e.rd32(unit + 0x14);
    if (!pd) return;
    const uint32_t w = e.rd32(pd + 0x28);
    if (!w || !e.rd32(w + 0x28)) return;
    const uint32_t er = e.h(0xE03A0, e.rd32(w + 0x30), e.rd32(w + 0x28), e.rd32(w + 0x2C), 0, 0);
    if (!er || e.rd32(er) != 0) return;
    const uint32_t sh = (uint32_t)(int32_t)(int16_t)(e.rd32(e.rd32(ct + 0x1C) + 0x80) & 0xFFFF);
    const uint32_t a = e.h(0xA3F4, 0, er, sh, 0, 0);
    if (a) e.wr32(c + 0x1C, e.h(0xA400, 0, 0, 0, 0, 0, er, a, 1));
    const uint32_t v = e.rd32(c + 0x1C);
    if (!e.h(0xA3D0, 0, unit, 0, 0, 0, 0xFFFFFFFFu)) {
        const uint32_t t[2] = {0xB20FB6B0u, 0xBEE};
        e.h(0xA9FA, 0, 0, 0, 0, 0, unit, 0, 1, 0);
        e.x(0xA9FA, t, 2);
    }
    e.h(0xADF0, 0, 0, 0, 0, 0, unit, 0, 0xFFFFFFFFu);
    e.h(0xAE08, 0, 0, 0, 0, 0, unit, 0, 0xFFFFFFFFu);
    const uint32_t lst = e.rd32(er + 4);
    for (int32_t i = 0; i < (int32_t)e.h(0xA418, 0, 0, 0, 0, 0, lst); i++) {
        const uint32_t node = e.h(0xA442, 0, 0, 0, 0, 0, lst, (uint32_t)i);
        if (e.h(0xA3D60, er, node, unit, 0, 0)) {   // retail tests the whole EAX (test eax,eax), not AL
            e.h(0xA3D0, 0, unit, node, 0, 0, 0xFFFFFFFFu);
            const uint32_t r = e.h(0xA3D00, 0, v, node, 0, 0, v);
            const uint32_t t[2] = {0xB20FB6B0u, 0xBFD};
            e.h(0xA9FA, 0, 0, 0, 0, 0, unit, node, r, 0);
            e.x(0xA9FA, t, 2);
        }
    }
}
}
