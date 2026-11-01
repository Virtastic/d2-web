// 1.13c D2Game+0x28BE0 -- skill hit loop over the targets of a skill (ECX = game, EDX = unit, [ebp+8] = skill id, [ebp+0xC] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_gp.h).
// Skill record out of range -> 0. rec = 0xA3F4(unit, skill); none -> 0. A = 0xAD12(rec), B = 0xACB2(rec); either zero -> 0x28170(rec, skill, unit; A, B), 0.
// Flags = 0xA79C(rec): low two bits both set -> 0x28170(.., A, B), 1; bit 0 clear -> 0. 0xDFA90(unit) nonzero -> 0. A type-0 unit: 0xE2310(game, 3); else
// 0xACE8(unit, 0) and unit +0x48 = 0x400. n = 0x26E40(unit; game, rec); for each of n: target = 0xA0F40(AD3C(rec); 3, 5; game, unit, 0, 0); none -> 0xAD0C(rec, -1) and the
// loop ends after its tail; else 0xAD0C(rec, target +0xC), a 0x70-byte request as in 0x289A0 (0xACD6, 0xDE5A0, record bits, 0xAC9A at record +0x138, 0x9F210), 0xDDDE0 and
// 0xDED30(game, unit, target). Each pass ends with 0xA78A(rec, 0xA79C(rec) with bit 13 flipped). Returns 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f28be0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    const uint32_t srec = e.skillRec(T, (int32_t)sk);
    if (!srec) return 0;
    const uint32_t rec = e.h(0xA3F4, 0, unit, sk);
    if (!rec) return 0;
    const uint32_t A = e.h(0xAD12, 0, 0, 0, 0, 0, rec);
    const uint32_t B = e.h(0xACB2, 0, 0, 0, 0, 0, rec);
    if (!A || !B) { e.h(0x28170, rec, sk, unit, 0, 0, A, B); return 0; }
    const uint32_t fl = e.h(0xA79C, 0, 0, 0, 0, 0, rec);
    if ((fl & 3) == 3) { e.h(0x28170, rec, sk, unit, 0, 0, A, B); return 1; }
    if (!(fl & 1)) return 0;
    if (e.h(0xDFA90, unit)) return 0;
    if (unit && e.rd32(unit) == 0) e.h(0xE2310, 0, 0, 0, 0, 0, game, 3);
    else { e.h(0xACE8, 0, 0, 0, 0, 0, unit, 0); e.wr32(unit + 0x48, 0x400); }
    int32_t n = (int32_t)e.h(0x26E40, 0, 0, 0, 0, unit, game, rec);
    for (int32_t i = 0; i < n; i++) {
        const uint32_t x = e.h(0xAD3C, 0, 0, 0, 0, 0, rec);
        const uint32_t tgt = e.h(0xA0F40, 3, x, 5, 0, 0, game, unit, 0, 0);
        if (!tgt) {
            e.h(0xAD0C, 0, 0, 0, 0, 0, rec, 0xFFFFFFFFu);
            n = 0;
        } else {
            e.h(0xAD0C, 0, 0, 0, 0, 0, rec, e.rd32(tgt + 0xC));
            const uint32_t r = e.h(0xACD6, 0, 0, 0, 0, 0, unit, sk, lv);
            const uint32_t f = e.h(0xDE5A0, tgt, 0, 0, 0, 0, game, r, 0);
            uint32_t blk[28] = {};
            uint32_t w16 = f & 0xFFFF;
            if (f & 1) {
                w16 |= (e.rd32(srec + 0x12C) >> 16) & 0xFFFF;
                blk[0] |= e.rd32(srec + 0x130);
                if (e.rd32(srec + 0x134)) blk[0x18] = e.rd32(srec + 0x134);
                blk[3] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(srec + 0x138), sk, lv);
                blk[1] = w16;
                e.hs(0x9F210, sk, 0, 0, 0, 0, { e.stk(blk), lv }, 0, 28);
            }
            e.h(0xDDDE0, unit, 0, 0, 0, 0, game, tgt);
            e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, tgt);
        }
        uint32_t f2 = e.h(0xA79C, 0, 0, 0, 0, 0, rec);
        f2 = (f2 & 0x2000) ? (f2 & ~0x2000u) : (f2 | 0x2000u);
        e.h(0xA78A, 0, 0, 0, 0, 0, rec, f2);
    }
    return 1;
}
}
