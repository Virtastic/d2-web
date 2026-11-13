// 1.13c D2Game+0x86280 -- ECX game, EDX unit, one stack word ctl (ret 4). No result. From ctl: c8 = ctl[+8], q = ctl[+0x14] (a local passed by pointer), rec = ctl[+0x1C] (a record of shorts).
// A type-1 unit whose pData[+0x54] is 3 or 0x13 and 0xB1BC0(EAX unit, ECX game; &q) answers v non-zero: 0xA496(unit, 0), request {4, unit, v, 0, 0, byte +0x15 = 0x64 (0x65 when the 0x1320(EAX unit[+4]) record's byte +0x104
// has a bit in bm[4], bm = [D2Game+0xF8BE0]), 0, 0} to 0x75E10(game, &req, 1). Done.
// Otherwise v = 0xB1BC0(...) again: non-zero and q < 0x14: seed [unit+0x20/0x24] steps *0x6AC690C5 + hi, low % 100 < rec short[+0x56 + 2 * game byte +0x6D]: 0xF2220(EAX 4, EDI unit; game, v). Else 0x1180(EAX 0x64, ECX unit+0x20) < 0x14:
// 0xF26B0(EAX 0, ECX unit; game, c8, 3), else 0xF22C0(EAX game, ESI unit; short +0x5C[b]). v zero or q >= 0x14: the same step, % 100 < short +0x62[b]: 0xF1D90(EAX short +0x68[b], ECX c8; game, unit, 2, short +0x6E[b]), else 0xF22C0(EAX game, ESI unit; 0x14).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void f86280(GE& e, uint32_t game, uint32_t unit, uint32_t ctl, uint32_t bm)
{
    const uint32_t c8 = e.rd32(ctl + 8), rec = e.rd32(ctl + 0x1C);
    volatile uint32_t q = e.rd32(ctl + 0x14);
    if (unit && e.rd32(unit) == 1) {
        const uint32_t pd = e.rd32(unit + 0x14);
        if (pd) {
            const uint32_t k = e.rd32(pd + 0x54);
            if (k == 3 || k == 0x13) {
                const uint32_t v = e.h(0xB1BC0, unit, game, 0, 0, 0, (uint32_t)(uintptr_t)&q);
                if (v) {
                    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
                    uint32_t st[8] = {4, unit, v, 0, 0, 0, 0, 0};
                    uint8_t bl = 0x64;
                    const uint32_t m = e.h(0x1320, e.rd32(unit + 4));
                    if (m && (e.rd8(m + 0x104) & e.rd32(bm + 0x10))) bl = 0x65;
                    st[5] = (uint32_t)bl << 8;
                    e.h(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)st, 1);
                    e.blk(0x75E10, (uint32_t)(uintptr_t)st, 8);
                    return;
                }
            }
        }
    }
    const uint32_t v = e.h(0xB1BC0, unit, game, 0, 0, 0, (uint32_t)(uintptr_t)&q);
    const uint32_t b = e.rd8(game + 0x6D);
    const uint64_t s = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(s >> 32)); e.wr32(unit + 0x20, (uint32_t)s);
    const int32_t rem = (int32_t)((uint32_t)s % 100);
    auto sh = [&](uint32_t off) { return (uint32_t)(int32_t)(int16_t)(e.rd32(rec + off + 2 * b) & 0xFFFF); };
    if (v && (int32_t)q < 0x14) {
        if (rem < (int32_t)sh(0x56)) {
            e.h(0xF2220, 4, 0, 0, 0, 0, game, v);
            e.y(0xF2220, game, unit);
            return;
        }
        const uint32_t r = e.h(0x1180, 0x64, unit + 0x20);
        if ((int32_t)r < 0x14) e.h(0xF26B0, 0, unit, 0, 0, 0, game, c8, 3);
        else e.h(0xF22C0, game, 0, 0, 0, unit, sh(0x5C));
        return;
    }
    if (rem < (int32_t)sh(0x62)) e.h(0xF1D90, sh(0x68), c8, 0, 0, 0, game, unit, 2, sh(0x6E));
    else e.h(0xF22C0, game, 0, 0, 0, unit, 0x14);
}
}
