// 1.13c D2Game+0xA1E80 -- ECX game, EDX unit, two stack words (ret 8): id (a1 unused). No result.
// id == -1: o = 0xA3B2(unit); non-zero: s = 0xA49C(o, string, 0xCBB), p = 0xA400(unit, o, 1); with 0 <= s < skill count, the record's byte +4 AND the byte at +0x14 of the bit-mask table (global
// D2Game+0xF8BE0) non-zero, and 0xDFA90(EAX unit) == 0: 0xA19A0(EAX unit; game, s, p, 1, 0, 0) then 0x9F3B0(EAX s, ECX game; unit, p, 1), done. Otherwise every node of the unit's
// chain (+0xDC, next +0x24) with byte 8 and word +0x14 == -1 goes to 0x8E6F0(EAX node).
// id > 0 (< skill count): record short +0x80 = v in [0, +0xC4 count), 0xA68E(unit, v) and q = 0xA5D4(unit, v) non-zero, r = 0xA712(q, 0x15F, 0), 0xA3F4(ECX unit, EDX id) non-zero and
// 0xA712(q, 0x15E, 0) == id: 0xA19A0(EAX unit; game, id, r, 1, 0, 0) then 0x9F3B0(EAX id, ECX game; unit, r, 0). Any other failure past the id test: 0x8E900(EAX unit, ECX game; 8).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fa1e80(GE& e, uint32_t game, uint32_t unit, uint32_t id, uint32_t dt, uint32_t bm)
{
    static const uint32_t z2[2] = {0, 0};   // stack words 4 and 5 of the 0xA19A0 calls
    if (id == 0xFFFFFFFFu) {
        const uint32_t o = e.h(0xA3B2, 0, 0, 0, 0, 0, unit);
        if (o) {
            const int32_t s = (int32_t)e.h(0xA49C, 0, 0, 0, 0, 0, o, 0xB20FB980u, 0xCBB);
            const uint32_t p = e.h(0xA400, 0, 0, 0, 0, 0, unit, o, 1);
            if (s >= 0) {
                const uint32_t sk = e.skillRec(dt, s);
                if (sk && (e.rd8(bm + 0x14) & e.rd8(sk + 4)) && !e.h(0xDFA90, unit)) {
                    e.h(0xA19A0, unit, 0, 0, 0, 0, game, (uint32_t)s, p, 1);
                    e.x(0xA19A0, z2, 2);
                    e.h(0x9F3B0, (uint32_t)s, game, 0, 0, 0, unit, p, 1);
                    return;
                }
            }
        }
        if (!unit) return;
        uint32_t n = e.rd32(unit + 0xDC);
        while (n) {
            const uint32_t nx = e.rd32(n + 0x24);
            if (e.rd8(n) == 8 && e.rd32(n + 0x14) == 0xFFFFFFFFu) e.h(0x8E6F0, n);
            n = nx;
        }
        return;
    }
    if ((int32_t)id <= 0) return;
    if (id < e.rd32(dt + 0xBA0)) {
        const uint32_t sk = e.skillRec(dt, (int32_t)id);
        if (sk) {
            const int16_t v = (int16_t)(e.rd32(sk + 0x80) & 0xFFFF);
            if (v >= 0 && (int32_t)v < (int32_t)e.rd32(dt + 0xC4) && e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)v)) {
                const uint32_t q = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)v);
                if (q) {
                    const uint32_t r = e.h(0xA712, 0, 0, 0, 0, 0, q, 0x15F, 0);
                    if (e.h(0xA3F4, 0, unit, id, 0, 0) && e.h(0xA712, 0, 0, 0, 0, 0, q, 0x15E, 0) == id) {
                        e.h(0xA19A0, unit, 0, 0, 0, 0, game, id, r, 1);
                        e.x(0xA19A0, z2, 2);
                        e.h(0x9F3B0, id, game, 0, 0, 0, unit, r, 0);
                        return;
                    }
                    e.h(0x8E900, unit, game, 0, 0, 0, 8);
                    return;
                }
            }
        }
    }
    e.h(0x8E900, unit, game, 0, 0, 0, 8);
}
}
