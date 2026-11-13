// 1.13c D2Game+0xACC30 -- EDX mode (6 doubles the divisor), five stack words (ret 0x14): unit, target, record ptr (byte +4 |= 2 when the target is left at 0 life), packed (hi word, lo word), unused; returns 0 or 1.
// Both pointers non-null; 0xA802(unit, hi, lo) = n must be positive; the unit's seed (+0x20/+0x24) takes one roll inline (x * 0x6AC690C5 + hi) and the roll mod 100 must be below n.
// Divisor d: target type 0 = 10; type 1 = 10 when 0xA7F6(target) answers, else 8 when 0xA580(0, target) or 0x22A80(EAX target, 2) answers (4 otherwise for types past 1), then
// 0xA42A(target, 100, 0) clamped to at least 1 goes to 0xAF5F0 (EAX) and, when that answers, d += 0x14D0(EAX that, ECX 100, EDX d). EDX == 6 doubles d.
// Hit points hp = 0xA42A(target, 6, 0); q = hp / d; with stat 0x24 >= 100 (clamped to 100) or > 0: q -= 0x14D0(EAX pct, 100, q). hp = max(hp - q, 0) goes to 0xA472(target, 6, hp, 0);
// hp <= 0 with a record sets its flag; q > 0 calls 0xA83E(target, 0x93, 0); result 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t facc30(GE& e, uint32_t mode, uint32_t unit, uint32_t tgt, uint32_t rec, uint32_t packed)
{
    if (!unit || !tgt) return 0;
    const int32_t n = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, unit, packed >> 16, packed);
    if (n <= 0) return 0;
    const uint64_t s = (uint64_t)e.rd32(unit + 0x20) * 0x6ac690c5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(s >> 32));
    e.wr32(unit + 0x20, (uint32_t)s);
    if ((int32_t)((uint32_t)s % 100u) >= n) return 0;
    int32_t d = 4;
    const uint32_t t = e.rd32(tgt);
    if (t == 0) d = 10;
    else if (t == 1) {
        if (e.h(0xA7F6, 0, 0, 0, 0, 0, tgt)) d = 10;
        else {
            if (e.h(0xA580, 0, 0, 0, 0, 0, 0, tgt) || e.h(0x22A80, tgt, 0, 0, 0, 0, 2)) d = 8;
            int32_t p = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tgt, 100, 0);
            if (p < 1) p = 1;
            const uint32_t r = e.h(0xAF5F0, (uint32_t)p);
            if (r) d += (int32_t)e.h(0x14D0, r, 100, (uint32_t)d);
        }
    }
    if (mode == 6) d *= 2;
    const int32_t hp0 = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tgt, 6, 0);
    int32_t q = hp0 / d;
    int32_t pct = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tgt, 0x24, 0);
    if (pct >= 100) pct = 100;
    if (pct > 0) q -= (int32_t)e.h(0x14D0, (uint32_t)pct, 100, (uint32_t)q);
    int32_t hp = hp0 - q;
    if (hp <= 0) hp = 0;
    e.h(0xA472, 0, 0, 0, 0, 0, tgt, 6, (uint32_t)hp, 0);
    if (hp <= 0 && rec) e.wr8(rec + 4, e.rd8(rec + 4) | 2);
    if (q > 0) e.h(0xA83E, 0, 0, 0, 0, 0, tgt, 0x93, 0);
    return 1;
}
}
