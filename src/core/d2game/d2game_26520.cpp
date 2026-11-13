// 1.13c D2Game+0x26520: skill 0x5A start. Fastcall ECX = game, EDX = unit, two stack words (the second, [esp+8] at entry, is the gate; the first is never read; ret 8).
// A zero gate does nothing. The strength starts at 1; a type-1 unit whose +0x14, +0x28 record resolves (0xE03A0 over +0x28/+0x2C/+0x30) to an owner
// with a nonzero 0xA3F4(owner, 0x10C) takes (0xA400(owner, that, 1) / 2 + 1), capped at 0xF, and only when that is above 1. A unit with 0xA718(unit) above
// 10 is back to 1. The call 0xA2800 takes EAX unit, EBX = 0xACFA(0x5A, 0), stack game, unit, 0x5A, 0, strength.
#include "d2game_seam4e.h"
extern "C" void d2g_26520(uint32_t game, uint32_t unit, uint32_t gate, const D2Seam* e)
{
    if (!gate) return;
    int32_t str = 1;
    uint32_t a, q, c;
    if (unit && d2rd32(unit) == 1 && (a = d2rd32(unit + 0x14)) && (q = d2rd32(a + 0x28)) && (c = d2rd32(q + 0x28))) {
        const uint32_t own = e->call(0xE03A0, d2rd32(q + 0x30), c, d2rd32(q + 0x2C));
        if (own) {
            const uint32_t p = e->call(0xA3F4, 0, own, 0x10C);
            if (p) {
                const int32_t v = (int32_t)e->call(0xA400, 0, 0, 0, 0, 0, own, p, 1);
                const int32_t n = v / 2 + 1;
                if (n > 1) str = n < 0xF ? n : 0xF;
            }
        }
    }
    if ((int32_t)e->call(0xA718, 0, 0, 0, 0, 0, unit) > 10) str = 1;
    const uint32_t b = e->call(0xACFA, 0, 0, 0, 0, 0, 0x5A, 0);
    e->call(0xA2800, unit, 0, 0, b, 0, game, unit, 0x5A, 0, (uint32_t)str);
}
