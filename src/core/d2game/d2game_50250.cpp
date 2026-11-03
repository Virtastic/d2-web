// D2Game+0x50250 -- SkillNec_ApplySkillEffectSlots: create the skill's effect object from the request record `s` (EDX) for a unit (ECX), no stack args, plain ret.
// s: +0 a, +4 b, +8 flag, +0xC flag2, +0x10 c, +0x14 d, +0x18 e, +0x1C stat, +0x20..+0x30 five stat ids, +0x38..+0x48 their values, +0x4C kind, +0x50..+0x58 three ids, +0x5C..+0x64 three table indexes.
// flag set: unit must exist, its type (dword +0) 1, 0xA514(unit) != 1, and 0x9F120(EAX = unit; k) non-zero, k = 0xA for kind 0x17, 0xC for 0x1B, 0xB for 0x38, else 0. Else: 0.
// v = 0 or (stat >= 0) 0x4E230(EDI = stat, EBX = s +0x34, ESI = unit), zero: 0. 0x4ECD0(a, b) zero: 0. Block {b, unit, c, d, e, stat, v, kind, flag ? D2Game+0x50130 : 0}:
// m = 0xA00B0(EAX = block), none: 0. flag: 0x500A0(EAX = c, EDX = unit; a). For the five stat ids (0 <= id < dt +0xBD4): r = 0x4E230(EDI = id, EBX = its value, ESI = unit); non-zero: 0xA6E2(m, id, r, 0).
// flag2: 0xA59E(unit, D2Game+0xFC1DC, 0x246). Then id0 (+0x50) >= 0 and idx0 (+0x5C) > 0: 0x37110(EDI = unit, EBX = a; 1, kind) and for each of the three: idx in 0..0x31 with table[idx] (D2Game+0x1077A8) non-zero:
// 0x37190(a, id, c, d, table[idx], 1, kind). Returns 1.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x50250
extern "C" uint32_t d2game_50250(uint32_t unit, uint32_t s, uint32_t dt, uint32_t gt, const D2Seam* e)
{
    const uint32_t flag = d2rd32(s + 8), kind = d2rd32(s + 0x4C);
    if (flag) {
        if (!unit || d2rd32(unit) != 1 || e->call(0xA514, 0, 0, 0, 0, 0, unit) == 1) return 0;
        const uint32_t k = kind == 0x17 ? 0xA : kind == 0x1B ? 0xC : kind == 0x38 ? 0xB : 0;
        if (!e->call(0x9F120, unit, 0, 0, 0, 0, k)) return 0;
    }
    int32_t stat = (int32_t)d2rd32(s + 0x1C);
    uint32_t v = 0;
    if (stat >= 0) {
        v = e->call(0x4E230, 0, 0, 0, d2rd32(s + 0x34), unit);
        if (!v) return 0;
    }
    if (!e->call(0x4ECD0, 0, 0, 0, 0, 0, d2rd32(s), d2rd32(s + 4))) return 0;
    uint32_t q[9] = {};
    q[0] = d2rd32(s + 4); q[1] = unit; q[2] = d2rd32(s + 0x10); q[3] = d2rd32(s + 0x14); q[4] = d2rd32(s + 0x18);
    q[5] = (uint32_t)stat; q[6] = v; q[7] = kind; q[8] = flag ? (0xB2000000u | 0x50130u) : 0;
    const uint32_t m = e->call(0xA00B0, (uint32_t)(uintptr_t)q);
    e->blk(0xA00B0, q, 9);
    if (!m) return 0;
    if (flag) e->call(0x500A0, d2rd32(s + 0x10), 0, unit, 0, 0, d2rd32(s));
    for (int i = 0; i < 5; i++) {
        const int32_t id = (int32_t)d2rd32(s + 0x20 + 4 * i);
        if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBD4)) continue;
        const uint32_t r = e->call(0x4E230, 0, 0, 0, d2rd32(s + 0x38 + 4 * i), unit);
        if (r) e->call(0xA6E2, 0, 0, 0, 0, 0, m, (uint32_t)id, r, 0);
    }
    if (d2rd32(s + 0xC)) e->call(0xA59E, 0, 0, 0, 0, 0, unit, 0xB2000000u | 0xFC1DCu, 0x246);
    if ((int32_t)d2rd32(s + 0x50) >= 0 && (int32_t)d2rd32(s + 0x5C) > 0) {
        e->call(0x37110, 0, 0, 0, d2rd32(s), 0, 1, kind);
        for (int i = 0; i < 3; i++) {
            if ((int32_t)d2rd32(s + 0x50) < 0 || (int32_t)d2rd32(s + 0x5C) <= 0) continue;
            const int32_t ix = (int32_t)d2rd32(s + 0x5C + 4 * i);
            if (ix < 0 || ix >= 0x32) continue;
            const uint32_t g = d2rd32(gt + 4 * (uint32_t)ix);
            if (!g) continue;
            e->call(0x37190, 0, 0, 0, 0, 0, d2rd32(s), d2rd32(s + 0x50 + 4 * i), d2rd32(s + 0x10), d2rd32(s + 0x14), g);
        }
    }
    return 1;
}
