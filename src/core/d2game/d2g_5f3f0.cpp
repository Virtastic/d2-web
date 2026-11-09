// 1.13c D2Game+0x5F3F0 -- quest event handler. ECX = quest q {+4 game, +9 active, +0xB, +0xC state, +0x14, +0x18 data,
// +0x1C list, word +0x9C, +0xE0 flag}, EDX = ev {+0 game, +8, +0xC player, +0x14 a, +0x18 b}; no result. From the 1.13c disassembly.
// b 0x70: active and state 0 -> state 1, 0x5EC30(q). b 0x71/0x72 and active: state <= 2 -> 0x35EB0(EAX 3); first time (+0xB)
// clears +0x14 and posts 0x591D0, and then (or after a state change) 0x58110, each through 0xDFCC0(EAX 0, EBX fn, EDI q+4; 0);
// then 0x5EC30(q). a 0x6D: a nonzero word +0x9C calls 0x33830(EAX player id or -1, ECX q+0x1C); state 2 checks the player's
// quest flags (A568 bit (+0xE0, 0) or (+0xE0, 1) == 1 returns), else 0x35EB0(3) and the same 591D0/58110 posts. b 0x79..0x7C,
// active and state < 5: 0xDFCC0(EAX 0, EBX 0x59120, EDI ev+0; ev+8), state 6, data+0x84 = 2, data+0x88 = 1 if clear.
#include "../fn/hookenv.h"
namespace d2g {
static void post5f3f0(HookEnv& e, uint32_t fn, uint32_t edi, uint32_t s0)
{
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | fn, 0, s0);
    e.edi(0xDFCC0, edi);
}
void f5f3f0(HookEnv& e, uint32_t q, uint32_t ev)
{
    const uint32_t a = e.rd32(ev + 0x14), b = e.rd32(ev + 0x18);
    if (b == 0x70) {
        if (e.rd8(q + 9) && !e.rd8(q + 0xC)) { e.wr8(q + 0xC, 1); e.h(0x5EC30, 0, 0, 0, 0, 0, q); }
    } else if ((b == 0x71 || b == 0x72) && e.rd8(q + 9)) {
        bool f = false;
        if (e.rd8(q + 0xC) <= 2) { e.h(0x35EB0, 3); f = true; }
        if (!e.rd8(q + 0xB)) {
            e.wr8(q + 0x14, 0); e.wr8(q + 0xB, 1);
            post5f3f0(e, 0x591D0, e.rd32(q + 4), 0);
            post5f3f0(e, 0x58110, e.rd32(q + 4), 0);
        } else if (f) post5f3f0(e, 0x58110, e.rd32(q + 4), 0);
        e.h(0x5EC30, 0, 0, 0, 0, 0, q);
    }
    if (a == 0x6D) {
        if (e.rd16s(q + 0x9C) & 0xFFFF) {
            const uint32_t pl = e.rd32(ev + 0xC);
            e.h(0x33830, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu, q + 0x1C);
        }
        if (e.rd8(q + 0xC) == 2) {
            const uint32_t game = e.rd32(ev);
            const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xC));
            const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
            if (e.h(0xA568, 0, 0, 0, 0, 0, fl, e.rd32(q + 0xE0), 0) == 1) return;
            if (e.h(0xA568, 0, 0, 0, 0, 0, fl, e.rd32(q + 0xE0), 1) == 1) return;
            e.h(0x35EB0, 3);
            if (!e.rd8(q + 0xB)) {
                e.wr8(q + 0x14, 0); e.wr8(q + 0xB, 1);
                post5f3f0(e, 0x591D0, e.rd32(q + 4), 0);
            }
            post5f3f0(e, 0x58110, e.rd32(q + 4), 0);
        }
    }
    if ((int32_t)b >= 0x79 && (int32_t)b <= 0x7C && e.rd8(q + 9) && e.rd8(q + 0xC) < 5) {
        post5f3f0(e, 0x59120, e.rd32(ev), e.rd32(ev + 8));
        e.wr8(q + 0xC, 6);
        const uint32_t d = e.rd32(q + 0x18);
        e.wr32(d + 0x84, 2);
        if (!e.rd32(d + 0x88)) e.wr32(d + 0x88, 1);
    }
}
}
