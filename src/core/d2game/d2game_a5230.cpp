// 1.13c D2Game+0xA5230 -- ECX a, EDX b (a unit), stack t (candidate unit), stack s (tally record), ret 8. Always returns 0.
// Rejects b == t, 0xDFA90(EAX t) non-zero, t null. A type-1 t with 0xA6F4(b, t) set and id = 0x18A0(EAX t) (-1 outside 0 <= id < dt[+0xA80], dt = [D2Game+0xF829C]) in
// 0x19A..0x19D, 0x19F, 0x1A0: s[8]++, done. Otherwise t[+0xC4] bit 2 and 0xE0A60(EAX t, ECX b; a) non-zero: with s[0] set, d = 0x1900(EAX s[0], EBX t): d <= 0x64: s[6]++;
// d < s[5]: s[4] = t, s[5] = d. Then d = 0x1900(EAX b, EBX t); d <= 0x400: s[7]++, d <= 0x64: s[3]++, d < s[2]: s[1] = t, s[2] = d; 0xA3C80(EAX d, ESI t; b) non-zero: s[9] = t.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fa5230(GE& e, uint32_t a, uint32_t b, uint32_t t, uint32_t s, uint32_t dt)
{
    if (b == t || e.h(0xDFA90, t) || !t) return 0;
    if (e.rd32(t) == 1 && e.h(0xA6F4, 0, 0, 0, 0, 0, b, t)) {
        int32_t id = (int32_t)e.h(0x18A0, t);
        if (id < 0 || id >= (int32_t)e.rd32(dt + 0xA80)) id = -1;
        const uint32_t k = (uint32_t)(id - 0x19A);
        if (k <= 6 && k != 4) { e.wr32(s + 0x20, e.rd32(s + 0x20) + 1); return 0; }
    }
    if (!((e.rd32(t + 0xC4) >> 2) & 1)) return 0;
    if (!e.h(0xE0A60, t, b, 0, 0, 0, a)) return 0;
    if (e.rd32(s)) {
        const int32_t d = (int32_t)e.h(0x1900, e.rd32(s), 0, 0, t, 0);
        if (d <= 0x64) e.wr32(s + 0x18, e.rd32(s + 0x18) + 1);
        if (d < (int32_t)e.rd32(s + 0x14)) { e.wr32(s + 0x10, t); e.wr32(s + 0x14, (uint32_t)d); }
    }
    const int32_t d = (int32_t)e.h(0x1900, b, 0, 0, t, 0);
    if (d > 0x400) return 0;
    e.wr32(s + 0x1C, e.rd32(s + 0x1C) + 1);
    if (d <= 0x64) e.wr32(s + 0xC, e.rd32(s + 0xC) + 1);
    if (d < (int32_t)e.rd32(s + 8)) { e.wr32(s + 4, t); e.wr32(s + 8, (uint32_t)d); }
    if (e.h(0xA3C80, (uint32_t)d, 0, 0, 0, t, b)) e.wr32(s + 0x24, t);
    return 0;
}
}
