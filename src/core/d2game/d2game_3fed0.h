// 1.13c D2Game+0x3FED0 (a missile heal-on-hit callback): ecx = pGame, edx = missile unit, plain ret.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: a null unit, class (+4) outside
// [0, missiles count) or a null missile row -> 2. s = import 0xB26A(unit); s outside [0, skills count) or a null skill row -> 2.
// Unit +0xC8 bit 10 set: o = 0xE03A0 (eax = unit +0x94, ecx = game, edx = unit +0x98); o non-null and 0xDFA90 (eax = o) zero: l = import
// 0xB0FC(unit); import 0xB28E(unit) equal to row +0x38: c = import 0xA42A(o, 6, 0) >> 8, m = import 0xA3A0(o) >> 8 (arithmetic); c below m:
// c += 0x14D0 (eax = import 0xAC9A(o, skill row +0x138, s, l), edx = m, ecx = 100; plain ret), capped at m; import 0xA472(o, 6, c << 8, 0);
// k = row.s16[+0x36] in (0, dt +0xBC0) -> import 0xA83E(o, k, 0). Every path past the row checks ends in 0x3F320 (ebx = unit, stack game;
// ret 4), whose result is the result.
#pragma once
#include <cstdint>

namespace d2g3fed0 {
template <class E> uint32_t HealHit(E& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t cls = (int32_t)e.rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xB6C)) return 2;
    const uint32_t row = (uint32_t)cls * 0x1A4u + e.rd32(dt + 0xB64);
    if (!row) return 2;
    const int32_t s = (int32_t)e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    if (s < 0 || s >= (int32_t)e.rd32(dt + 0xBA0)) return 2;
    const uint32_t sk = (uint32_t)s * 0x23Cu + e.rd32(dt + 0xB98);
    if (!sk) return 2;
    if ((e.rd32(unit + 0xC8) >> 10) & 1) {
        const uint32_t o = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
        if (o && !e.h(0xDFA90, o)) {
            const uint32_t l = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
            if (e.h(0xB28E, 0, 0, 0, 0, 0, unit) == e.rd32(row + 0x38)) {
                int32_t c = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, o, 6, 0) >> 8;
                const int32_t m = (int32_t)e.h(0xA3A0, 0, 0, 0, 0, 0, o) >> 8;
                if (c < m) {
                    const uint32_t a = e.h(0xAC9A, 0, 0, 0, 0, 0, o, e.rd32(sk + 0x138), (uint32_t)s, l);
                    c += (int32_t)e.h(0x14D0, a, 100, (uint32_t)m);
                    if (c >= m) c = m;
                    e.h(0xA472, 0, 0, 0, 0, 0, o, 6, (uint32_t)c << 8, 0);
                    const int32_t k = e.rd16s(row + 0x36);
                    if (k > 0 && k < (int32_t)e.rd32(dt + 0xBC0)) e.h(0xA83E, 0, 0, 0, 0, 0, o, (uint32_t)k, 0);
                }
            }
        }
    }
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
